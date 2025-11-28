# 🚀 M5Paper Wetter-Display - Schnellstart

5-Minuten Setup für dein Wetter-Display!

## ⚡ Schnellstart (Copy & Paste)

### 1️⃣ Icons generieren

```bash
cd /c/Users/btrom/source/repos/epdiy/scripts
python download_weather_icons.py
copy ..\weather_icons.h ..\..\..\esphome_components\
```

### 2️⃣ Secrets-Datei erstellen

Erstelle `esphome_components/secrets.yaml`:

```yaml
wifi_ssid: "DeinWiFi"
wifi_password: "DeinPasswort"
```

### 3️⃣ Wetter-Entitäten anpassen

Öffne [m5stack-papers3-weather.yaml](m5stack-papers3-weather.yaml) und ersetze:

```yaml
# Zeile ~82: Deine Home Assistant Wetter-Entität
entity_id: weather.home  # ← zu z.B. weather.openweathermap

# Zeile ~104: Dein Außentemperatursensor (optional)
entity_id: sensor.outdoor_temperature  # ← zu z.B. sensor.garten_temp
```

**Deine Entitäten finden:**
- Home Assistant → Entwicklerwerkzeuge → Zustände
- Suche nach `weather.` oder `sensor.`

### 4️⃣ Flashen!

```bash
cd /c/Users/btrom/source/repos/esphome_components
esphome run m5stack-papers3-weather.yaml
```

- Wähle USB-Port
- Warte ~5 Minuten
- Fertig! ✅

## 🎨 Beispiel-Screenshots

Das Display zeigt:

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
│  Feucht.    Außen    Luftdruck     │
│   65%      22.3°C    1013 hPa      │
│                                     │
│        Wind: 12.5 km/h              │
├─────────────────────────────────────┤
│ Batterie 4.1V  WiFi: OK  14:30     │
│ 85%                    CPU: 42°C   │
└─────────────────────────────────────┘
```

## 🔧 Häufigste Anpassungen

### Zeitzone ändern

```yaml
# Zeile ~166
timezone: Europe/Berlin  # → Europe/Vienna, Europe/Zurich, etc.
```

### Update-Intervall ändern

```yaml
# Zeile ~234
interval: 6h  # → 1h, 30min, 15min
```

### Deep Sleep (Batterie-Sparmodus)

```yaml
# Am Ende der Datei hinzufügen:
deep_sleep:
  run_duration: 10s
  sleep_duration: 30min
```

**Achtung:** Keine OTA-Updates im Deep Sleep möglich!

## 📱 Home Assistant Automation

Automatisches Update bei Wetteränderung:

```yaml
# Home Assistant automations.yaml
automation:
  - alias: M5Paper Weather Update
    trigger:
      platform: state
      entity_id: weather.home
    action:
      service: esphome.m5papers3_weather_update_display
```

## ❓ Probleme?

### Display bleibt schwarz
```bash
esphome logs m5stack-papers3-weather.yaml
```
Prüfe auf Fehler im Log.

### "Entity not found"
- Falsche `entity_id` in der Konfiguration
- Prüfe in HA unter Entwicklerwerkzeuge → Zustände

### Keine Verbindung zu Home Assistant
```yaml
# Prüfe WiFi-Konfiguration
wifi:
  ssid: !secret wifi_ssid      # ← Korrekt?
  password: !secret wifi_password
```

## 📚 Vollständige Anleitung

Siehe [WEATHER_DISPLAY_GUIDE.md](WEATHER_DISPLAY_GUIDE.md) für:
- Erweiterte Konfiguration
- Custom Icons
- Batterie-Optimierung
- Touch-Steuerung
- Home Assistant Services

## ✨ Was als Nächstes?

- [ ] 3-Tages-Vorhersage hinzufügen
- [ ] Temperatur-Graph anzeigen
- [ ] Unwetter-Warnungen
- [ ] Multi-Standort Wetter
- [ ] Custom Icons designen

**Viel Spaß mit deinem Wetter-Display!** 🌤️
