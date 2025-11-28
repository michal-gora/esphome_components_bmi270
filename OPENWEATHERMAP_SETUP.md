# ☀️ M5Paper OpenWeatherMap Display - Setup

Deine Konfiguration ist bereits für OpenWeatherMap optimiert!

## ✅ Was angezeigt wird

```
┌─────────────────────────────────────┐
│      Montag, 28. Januar 2025        │
│             14:30                   │
├─────────────────────────────────────┤
│                                     │
│   ☁️        23.5°C                  │
│           Wolkig                    │
│                                     │
├─────────────────────────────────────┤
│  Feucht.    Gefühlt   Luftdruck    │
│   65%       24.2°C    1013 hPa     │
│                                     │
│   Wind: 12.5 km/h    UV: 3.2       │
│       Bewölkung: 45%                │
├─────────────────────────────────────┤
│ Batterie 4.1V  WiFi: OK  14:30     │
│ 85%                    CPU: 42°C   │
└─────────────────────────────────────┘
```

## 📊 OpenWeatherMap Sensoren (verwendet)

Deine Konfiguration nutzt folgende Sensoren:

✅ `sensor.openweathermap_temperature` - Haupttemperatur
✅ `sensor.openweathermap_feels_like_temperature` - Gefühlte Temperatur
✅ `sensor.openweathermap_humidity` - Luftfeuchtigkeit
✅ `sensor.openweathermap_pressure` - Luftdruck
✅ `sensor.openweathermap_wind_speed` - Windgeschwindigkeit
✅ `sensor.openweathermap_uv_index` - UV-Index
✅ `sensor.openweathermap_cloud_coverage` - Bewölkung
✅ `sensor.openweathermap_condition` - Wetterzustand (für Icon)

**Zusätzlich verfügbar (nicht verwendet):**
- `sensor.openweathermap_wind_gust` - Windböen
- `sensor.openweathermap_visibility` - Sichtweite
- `sensor.openweathermap_dew_point` - Taupunkt
- `sensor.openweathermap_rain` - Regenintensität
- `sensor.openweathermap_snow` - Schneeintensität

## 🚀 Schnellstart

### 1. Icons generieren

```bash
cd /c/Users/btrom/source/repos/epdiy/scripts
python download_weather_icons.py
copy ..\weather_icons.h ..\..\esphome_components\
```

### 2. Secrets-Datei erstellen

Erstelle `esphome_components/secrets.yaml`:

```yaml
wifi_ssid: "DeinWiFi"
wifi_password: "DeinPasswort"
```

### 3. Flashen!

```bash
cd /c/Users/btrom/source/repos/esphome_components
esphome run m5stack-papers3-weather.yaml
```

**Das war's!** Alle OpenWeatherMap Sensoren sind bereits konfiguriert.

## 🎨 Anpassungen

### Zeitzone ändern

Zeile ~176 in `m5stack-papers3-weather.yaml`:

```yaml
time:
  - platform: homeassistant
    timezone: Europe/Berlin  # ← ANPASSEN
```

**Beliebte Zeitzonen:**
- `Europe/Berlin`
- `Europe/Vienna` (Österreich)
- `Europe/Zurich` (Schweiz)
- `Europe/Paris` (Frankreich)
- `Europe/Amsterdam` (Niederlande)

### Update-Intervall ändern

Zeile ~234:

```yaml
interval:
  - interval: 6h  # ← z.B. zu 1h, 30min, 15min ändern
    then:
      - component.update: weather_display
```

**Empfehlungen:**
- **6h**: Normaler Betrieb, beste Batterie-Laufzeit
- **1h**: Häufigere Updates, gute Balance
- **15min**: Sehr aktuell, höherer Stromverbrauch

### Weitere Sensoren hinzufügen

#### Sichtweite anzeigen

In der Sensor-Sektion ist bereits `weather_visibility` definiert.
Füge im Display-Lambda hinzu:

```yaml
// Nach Bewölkung (Zeile ~440)
if (id(weather_visibility).has_state()) {
  it.printf(SCREEN_W/2, extra_y+80, id(font_small), TextAlign::TOP_CENTER,
            "Sicht: %.1f km", id(weather_visibility).state);
}
```

#### Taupunkt anzeigen

Sensor hinzufügen:

```yaml
sensor:
  # ... bestehende Sensoren ...
  - platform: homeassistant
    id: weather_dew_point
    entity_id: sensor.openweathermap_dew_point
```

Im Display:

```yaml
if (id(weather_dew_point).has_state()) {
  it.printf(100, 500, id(font_small), "Taupunkt: %.1f°C",
            id(weather_dew_point).state);
}
```

#### Regenintensität anzeigen

Sensor hinzufügen:

```yaml
sensor:
  - platform: homeassistant
    id: weather_rain
    entity_id: sensor.openweathermap_rain
```

Im Display (nur bei Regen):

```yaml
if (id(weather_rain).has_state() && id(weather_rain).state > 0) {
  it.printf(SCREEN_W/2, 820, id(font_medium), TextAlign::TOP_CENTER,
            "Regen: %.1f mm/h", id(weather_rain).state);
}
```

## 🌡️ Wetter-Bedingungen & Icons

OpenWeatherMap liefert diese Bedingungen (bereits im Code unterstützt):

| Bedingung | Icon | Beschreibung |
|-----------|------|--------------|
| `sunny` / `clear` | ☀️ | Sonnig/Klar |
| `cloudy` | ☁️ | Bewölkt |
| `partlycloudy` | ⛅ | Teilweise bewölkt |
| `rainy` / `rain` | 🌧️ | Regen |
| `pouring` | ⛈️ | Starkregen |
| `snowy` / `snow` | ❄️ | Schnee |
| `fog` / `foggy` | 🌫️ | Nebel |
| `lightning` | ⚡ | Gewitter |

## 🔋 Batterie-Optimierung

### Deep Sleep aktivieren

Füge am Ende der YAML-Datei hinzu:

```yaml
deep_sleep:
  id: deep_sleep_control
  run_duration: 10s
  sleep_duration: 30min  # Wacht alle 30 Min auf

esphome:
  on_boot:
    then:
      - component.update: weather_display
      - delay: 5s
      - deep_sleep.enter: deep_sleep_control
```

**Batterie-Laufzeit:**
- **Ohne Deep Sleep**: ~2-3 Tage (mit 6h Updates)
- **Mit Deep Sleep (30min)**: ~2-3 Wochen
- **Mit Deep Sleep (1h)**: ~4-6 Wochen

⚠️ **Achtung:** Im Deep Sleep sind keine OTA-Updates möglich!

### WiFi Power-Save

Bereits aktiviert in Zeile ~68:

```yaml
wifi:
  power_save_mode: LIGHT
```

## 🏠 Home Assistant Automationen

### Automatisches Update bei Wetteränderung

```yaml
# In Home Assistant: automations.yaml
automation:
  - alias: "M5Paper: Update bei Wetteränderung"
    trigger:
      - platform: state
        entity_id: sensor.openweathermap_condition
    action:
      - service: esphome.m5papers3_weather_update_display
```

### Warnung bei hohem UV-Index

```yaml
automation:
  - alias: "M5Paper: UV-Warnung"
    trigger:
      - platform: numeric_state
        entity_id: sensor.openweathermap_uv_index
        above: 7
    action:
      - service: esphome.m5papers3_weather_play_tone
        data:
          rtttl_string: "uv_warning:d=4,o=5,b=140:16c6,16p,16c6,16p,16c6"
```

### Tägliches Voll-Refresh (gegen Ghosting)

Bereits eingebaut! Zeile ~234:

```yaml
interval:
  - interval: 6h
    then:
      - component.update: weather_display
```

## 📱 ESPHome Services

Das Display bietet diese Services in Home Assistant:

### `esphome.m5papers3_weather_update_display`

Aktualisiert das Display sofort.

```yaml
service: esphome.m5papers3_weather_update_display
```

### `esphome.m5papers3_weather_play_tone`

Spielt einen Signalton.

```yaml
service: esphome.m5papers3_weather_play_tone
data:
  rtttl_string: "beep:d=4,o=5,b=100:16e6,16e6"
```

## 🎯 Erweiterte Layouts

### Vorhersage für morgen anzeigen

Die Forecast-Sensoren sind bereits definiert (Zeile ~154-162).

Im Display hinzufügen:

```yaml
// Vorhersage-Bereich
if (id(weather_forecast_0).has_state()) {
  it.printf(100, 820, id(font_medium), "Morgen:");
  it.printf(100, 860, id(font_small), "%s",
            id(weather_forecast_0).state.c_str());

  if (id(weather_forecast_temp_0).has_state()) {
    it.printf(300, 860, id(font_small), "%s°C",
              id(weather_forecast_temp_0).state.c_str());
  }
}
```

### Windböen anzeigen

```yaml
// Wind mit Böen
if (id(weather_wind_speed).has_state()) {
  std::string wind_text = "Wind: " +
                          to_string((int)id(weather_wind_speed).state) +
                          " km/h";

  if (id(weather_wind_gust).has_state()) {
    wind_text += " (Böen: " +
                 to_string((int)id(weather_wind_gust).state) +
                 ")";
  }

  it.printf(SCREEN_W/2, 730, id(font_small),
            TextAlign::TOP_CENTER, wind_text.c_str());
}
```

## 🐛 Fehlerbehebung

### Display zeigt "unavailable"

**Grund:** Home Assistant ist nicht verbunden oder Sensor existiert nicht.

**Lösung:**
1. Prüfe WiFi-Verbindung
2. Logs anschauen:
   ```bash
   esphome logs m5stack-papers3-weather.yaml
   ```
3. In Home Assistant: Entwicklerwerkzeuge → Zustände
   - Sind alle OpenWeatherMap Sensoren verfügbar?

### Icons werden nicht angezeigt

1. **Header-Datei kopiert?**
   ```bash
   dir c:\Users\btrom\source\repos\esphome_components\weather_icons.h
   ```

2. **Neu kompilieren:**
   ```bash
   esphome clean m5stack-papers3-weather.yaml
   esphome compile m5stack-papers3-weather.yaml
   ```

### Falsche Zeitzone

Zeile ~176 anpassen:

```yaml
time:
  - platform: homeassistant
    timezone: Europe/Berlin  # ← HIER ÄNDERN
```

### Batterie entlädt sich zu schnell

1. **Update-Intervall erhöhen** (Zeile ~234):
   ```yaml
   interval: 6h  # statt z.B. 15min
   ```

2. **Deep Sleep aktivieren** (siehe oben)

3. **WiFi Power-Save prüfen** (Zeile ~68):
   ```yaml
   wifi:
     power_save_mode: LIGHT  # oder HIGH
   ```

## 📚 Nächste Schritte

- [ ] Teste die Grund-Konfiguration
- [ ] Passe Zeitzone an
- [ ] Probiere verschiedene Update-Intervalle
- [ ] Füge Vorhersage hinzu
- [ ] Aktiviere Deep Sleep für lange Laufzeit
- [ ] Erstelle Home Assistant Automationen

**Viel Erfolg mit deinem Wetter-Display!** 🌤️

---

**Weitere Hilfe:**
- [Vollständige Anleitung](WEATHER_DISPLAY_GUIDE.md)
- [Schnellstart](WEATHER_QUICKSTART.md)
- [ESPHome Dokumentation](https://esphome.io/)
- [OpenWeatherMap Integration](https://www.home-assistant.io/integrations/openweathermap/)
