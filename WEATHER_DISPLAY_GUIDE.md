# M5Stack PaperS3 Wetter-Display Anleitung

Komplette Anleitung zum Einrichten eines Wetter-Displays auf dem M5Stack PaperS3 mit ESPHome und Home Assistant.

## 📋 Voraussetzungen

- M5Stack PaperS3 E-Ink Display
- Home Assistant Installation mit funktionierender Wetter-Integration
- ESPHome installiert und konfiguriert
- Python 3.x für Icon-Generierung

## 🚀 Schnellstart

### Schritt 1: Wetter-Icons generieren

```bash
# In das Scripts-Verzeichnis wechseln
cd c:\Users\btrom\source\repos\epdiy\scripts

# Icons generieren und konvertieren
python download_weather_icons.py
```

Dies erstellt:
- PNG-Icons in `scripts/weather_icons/`
- Header-Datei `weather_icons.h` im epdiy-Root-Verzeichnis

### Schritt 2: Icons zu ESPHome kopieren

```bash
# Icons in ESPHome-Konfigurationsverzeichnis kopieren
copy ..\weather_icons.h c:\Users\btrom\source\repos\esphome_components\
```

### Schritt 3: ESPHome-Konfiguration anpassen

Öffne `m5stack-papers3-weather.yaml` und passe folgende Werte an:

#### 3.1 WiFi-Zugangsdaten

Erstelle/bearbeite `secrets.yaml` in deinem ESPHome-Verzeichnis:

```yaml
wifi_ssid: "DeinWiFi-Name"
wifi_password: "DeinWiFi-Passwort"
```

#### 3.2 Wetter-Entitäten

Ersetze die Platzhalter mit deinen echten Home Assistant Entitäten:

```yaml
sensor:
  - platform: homeassistant
    id: weather_temperature
    entity_id: weather.home  # ← HIER ANPASSEN

  - platform: homeassistant
    id: outdoor_temperature
    entity_id: sensor.outdoor_temperature  # ← HIER ANPASSEN
```

**So findest du deine Entitäten:**

1. Öffne Home Assistant
2. Gehe zu Entwicklerwerkzeuge → Zustände
3. Suche nach deiner Wetter-Integration (z.B. `weather.home`, `weather.openweathermap`)
4. Suche nach Temperatur-Sensoren (z.B. `sensor.garten_temperature`)

#### 3.3 Zeitzone

```yaml
time:
  - platform: homeassistant
    id: ha_time
    timezone: Europe/Berlin  # ← HIER ANPASSEN (z.B. Europe/Vienna, Europe/Zurich)
```

### Schritt 4: Firmware kompilieren und flashen

```bash
# ESPHome-Firmware kompilieren
esphome compile m5stack-papers3-weather.yaml

# Firmware hochladen (beim ersten Mal per USB)
esphome upload m5stack-papers3-weather.yaml
```

**Beim ersten Mal:**
- Verbinde M5Paper per USB
- Wähle den COM-Port aus
- Warte auf Upload-Abschluss (~5 Minuten)

**Danach:** Over-The-Air (OTA) Updates möglich!

### Schritt 5: In Home Assistant integrieren

1. Home Assistant sollte das Gerät automatisch erkennen
2. Gehe zu Einstellungen → Geräte & Dienste → Integrationen
3. Klicke auf "M5Paper Weather Display"
4. Konfiguration abschließen

## 🎨 Anpassungen

### Display-Layout ändern

Öffne `m5stack-papers3-weather.yaml` und bearbeite den `lambda`-Bereich im Display-Abschnitt:

```yaml
display:
  - platform: ed047tc1
    lambda: |-
      // Hier kannst du Positionen, Schriftgrößen, etc. ändern
      it.printf(x, y, id(font), "Text");
```

### Weitere Wetter-Daten anzeigen

Füge zusätzliche Sensoren hinzu:

```yaml
sensor:
  # UV-Index
  - platform: homeassistant
    id: weather_uv_index
    entity_id: sensor.uv_index

  # Niederschlagsmenge
  - platform: homeassistant
    id: weather_precipitation
    entity_id: sensor.precipitation
```

Dann im Display-Lambda:

```yaml
// UV-Index anzeigen
if (id(weather_uv_index).has_state()) {
  it.printf(100, 600, id(font_medium), "UV: %.0f", id(weather_uv_index).state);
}
```

### Update-Intervalle anpassen

```yaml
interval:
  # Häufigere Updates (mehr Batterieverbrauch)
  - interval: 10min  # statt 6h
    then:
      - component.update: weather_display
```

**Empfohlene Intervalle:**
- **6 Stunden**: Normaler Betrieb, schont Batterie
- **1 Stunde**: Häufigere Updates bei schnellen Wetteränderungen
- **15 Minuten**: Maximale Aktualität (höherer Stromverbrauch)

### Eigene Wetter-Icons verwenden

1. Erstelle 128x128 PNG-Bilder für jede Wetterbedingung
2. Speichere sie in `epdiy/scripts/weather_icons/`
3. Benenne sie wie die bestehenden Icons:
   - `sunny.png`
   - `cloudy.png`
   - `rainy.png`
   - etc.
4. Führe `download_weather_icons.py` erneut aus

**Icon-Quellen:**
- [Material Design Icons](https://materialdesignicons.com/)
- [Weather Icons](https://erikflowers.github.io/weather-icons/)
- [Flaticon Weather](https://www.flaticon.com/search?word=weather)

## 🔧 Erweiterte Konfiguration

### Batterie-Management optimieren

Deep Sleep aktivieren (extrem lange Batterie-Laufzeit):

```yaml
esphome:
  on_boot:
    then:
      - component.update: weather_display
      - delay: 5s
      - deep_sleep.enter: deep_sleep_control

deep_sleep:
  id: deep_sleep_control
  run_duration: 10s
  sleep_duration: 30min  # Wacht alle 30 Min auf
```

**Achtung:** Im Deep Sleep ist keine OTA-Aktualisierung möglich!

### Touch-Steuerung erweitern

```yaml
touchscreen:
  on_touch:
    - lambda: |-
        // Obere Hälfte: Display aktualisieren
        if (touch.y < 480) {
          id(weather_display).update();
        }
        // Untere Hälfte: Ton abspielen
        else {
          id(buzzer).play("beep:d=4,o=5,b=100:16e6");
        }
```

### Home Assistant Automationen

#### Automatisches Update bei Wetteränderung

In Home Assistant `automations.yaml`:

```yaml
automation:
  - alias: "M5Paper: Update bei Wetteränderung"
    trigger:
      - platform: state
        entity_id: weather.home
    action:
      - service: esphome.m5papers3_weather_update_display
```

#### Warnung bei extremem Wetter

```yaml
automation:
  - alias: "M5Paper: Unwetterwarnung"
    trigger:
      - platform: state
        entity_id: weather.home
        attribute: alert
    condition:
      - condition: template
        value_template: "{{ trigger.to_state.attributes.alert != none }}"
    action:
      - service: esphome.m5papers3_weather_play_tone
        data:
          rtttl_string: "alarm:d=4,o=5,b=140:16c6,16c6,16c6,8p"
      - service: notify.mobile_app
        data:
          message: "Unwetterwarnung auf M5Paper angezeigt!"
```

## 📊 Wetter-Integration Beispiele

### OpenWeatherMap

```yaml
# Home Assistant configuration.yaml
weather:
  - platform: openweathermap
    api_key: !secret openweathermap_api_key
    mode: freedaily
```

### Met.no (kostenlos, keine API-Key nötig)

```yaml
weather:
  - platform: met
    name: Home
```

### DarkSky / Weather.com

```yaml
weather:
  - platform: darksky
    api_key: !secret darksky_api_key
    mode: daily
```

## 🐛 Fehlerbehebung

### Display zeigt nichts an

1. **Log prüfen:**
   ```bash
   esphome logs m5stack-papers3-weather.yaml
   ```

2. **Display-Pins überprüfen:**
   - Sind alle Pin-Definitionen korrekt?
   - Siehe Hardware-Spezifikation im YAML

3. **Stromversorgung:**
   - Ist die Batterie geladen?
   - Funktioniert USB-Stromversorgung?

### Icons werden nicht angezeigt

1. **Header-Datei vorhanden?**
   ```bash
   dir c:\Users\btrom\source\repos\esphome_components\weather_icons.h
   ```

2. **Includes richtig gesetzt?**
   ```yaml
   esphome:
     includes:
       - weather_icons.h
   ```

3. **Kompilierung neu versuchen:**
   ```bash
   esphome clean m5stack-papers3-weather.yaml
   esphome compile m5stack-papers3-weather.yaml
   ```

### Wetter-Daten nicht verfügbar

1. **Home Assistant Verbindung:**
   - Ist API in ESPHome aktiviert?
   - Ist das Gerät mit WiFi verbunden?

2. **Entitäten prüfen:**
   ```yaml
   # Im Log erscheinen Warnungen bei falschen Entitäten
   sensor:
     - platform: homeassistant
       entity_id: weather.FALSCHE_ENTITAET  # ← Fehler im Log
   ```

3. **Sensoren in HA überprüfen:**
   - Entwicklerwerkzeuge → Zustände
   - Sind die Wetter-Entitäten verfügbar?

### Hoher Batterieverbrauch

1. **Update-Intervall reduzieren:**
   ```yaml
   interval:
     - interval: 6h  # statt 15min
   ```

2. **WiFi Power-Save aktivieren:**
   ```yaml
   wifi:
     power_save_mode: LIGHT
   ```

3. **Deep Sleep verwenden:**
   Siehe "Batterie-Management" oben

## 📱 Services für Home Assistant

Das Display registriert folgende Services:

### `esphome.m5papers3_weather_update_display`

Aktualisiert das Display manuell.

```yaml
service: esphome.m5papers3_weather_update_display
```

### `esphome.m5papers3_weather_play_tone`

Spielt einen RTTTL-Ton ab.

```yaml
service: esphome.m5papers3_weather_play_tone
data:
  rtttl_string: "scale:d=4,o=5,b=100:c,d,e,f,g,a,b,c6"
```

**RTTTL-Beispiele:**
- Alarm: `alarm:d=4,o=5,b=140:16c6,16c6,16c6`
- Melodie: `melody:d=4,o=5,b=125:16e,16e,16f,16g,16g,16f,16e,16d`
- Star Wars: `StarWars:d=4,o=5,b=45:32p,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6`

## 🎯 Nächste Schritte

### Grafische Vorhersage

Füge eine 3-Tages-Vorhersage mit Icons hinzu:

```yaml
text_sensor:
  - platform: homeassistant
    id: forecast_day1
    entity_id: weather.home
    attribute: forecast[0].condition

  - platform: homeassistant
    id: forecast_day2
    entity_id: weather.home
    attribute: forecast[1].condition
```

### Historische Daten / Graphen

Zeichne Temperatur-Verlauf als Liniengrafik:

```yaml
lambda: |-
  // Beispiel: Einfacher Temperatur-Graph
  std::vector<float> temps = {20.5, 21.0, 22.3, 23.1, 22.8};
  for (int i = 0; i < temps.size() - 1; i++) {
    int x1 = 50 + i * 100;
    int y1 = 400 - (temps[i] - 15) * 10;
    int x2 = 50 + (i+1) * 100;
    int y2 = 400 - (temps[i+1] - 15) * 10;
    it.line(x1, y1, x2, y2);
  }
```

### Multi-Standort Wetter

Zeige Wetter für mehrere Orte:

```yaml
sensor:
  - platform: homeassistant
    id: weather_berlin
    entity_id: weather.berlin

  - platform: homeassistant
    id: weather_muenchen
    entity_id: weather.muenchen
```

## 📚 Ressourcen

- [EPDiy Dokumentation](https://github.com/vroland/epdiy)
- [ESPHome Dokumentation](https://esphome.io/)
- [Home Assistant Wetter-Integrationen](https://www.home-assistant.io/integrations/#weather)
- [M5Stack PaperS3 Hardware](https://docs.m5stack.com/en/core/PaperS3)
- [RTTTL Ringtone Format](https://en.wikipedia.org/wiki/Ring_Tone_Transfer_Language)

## 🤝 Support

Bei Problemen oder Fragen:

1. Überprüfe die Logs: `esphome logs m5stack-papers3-weather.yaml`
2. Schaue in die [ESPHome Community](https://community.home-assistant.io/c/esphome)
3. Prüfe die [epdiy Issues](https://github.com/vroland/epdiy/issues)

## 📄 Lizenz

Diese Anleitung und die Beispiel-Konfiguration sind unter MIT-Lizenz verfügbar.
