# 🌤️ M5Stack PaperS3 Wetter-Display

Komplettes Wetter-Display für M5Stack PaperS3 mit OpenWeatherMap Integration.

## 📁 Dateien

| Datei | Beschreibung |
|-------|-------------|
| [m5stack-papers3-weather.yaml](m5stack-papers3-weather.yaml) | **Haupt-Konfiguration** - Fertig für OpenWeatherMap |
| [OPENWEATHERMAP_SETUP.md](OPENWEATHERMAP_SETUP.md) | **⭐ START HIER** - Setup-Anleitung für OpenWeatherMap |
| [WEATHER_QUICKSTART.md](WEATHER_QUICKSTART.md) | 5-Minuten Schnellstart |
| [WEATHER_DISPLAY_GUIDE.md](WEATHER_DISPLAY_GUIDE.md) | Vollständige Anleitung |
| `weather_icons.h` | Generierte Wetter-Icons (wird erstellt) |
| `secrets.yaml` | WiFi-Zugangsdaten (musst du erstellen) |

## ⚡ 2-Minuten Setup

```bash
# 1. Icons generieren
cd /c/Users/btrom/source/repos/epdiy/scripts
python download_weather_icons.py
copy ..\weather_icons.h ..\..\esphome_components\

# 2. Secrets erstellen
cd /c/Users/btrom/source/repos/esphome_components
echo "wifi_ssid: \"DeinWiFi\"" > secrets.yaml
echo "wifi_password: \"DeinPasswort\"" >> secrets.yaml

# 3. Flashen
esphome run m5stack-papers3-weather.yaml
```

**Fertig!** Das Display zeigt jetzt:
- 🌡️ Aktuelle Temperatur & Gefühlte Temperatur
- 💧 Luftfeuchtigkeit
- 🌬️ Wind & UV-Index
- ☁️ Bewölkung & Luftdruck
- 🔋 Batteriestatus
- ⏰ Datum & Uhrzeit

## 📊 OpenWeatherMap Sensoren

Die Konfiguration nutzt automatisch diese Sensoren:

✅ `sensor.openweathermap_temperature`
✅ `sensor.openweathermap_feels_like_temperature`
✅ `sensor.openweathermap_humidity`
✅ `sensor.openweathermap_pressure`
✅ `sensor.openweathermap_wind_speed`
✅ `sensor.openweathermap_uv_index`
✅ `sensor.openweathermap_cloud_coverage`
✅ `sensor.openweathermap_condition`

**Keine manuelle Anpassung nötig!**

## 🎨 Was wird angezeigt?

```
┌─────────────────────────────────────┐
│      Montag, 28. Januar 2025        │
│             14:30                   │
├─────────────────────────────────────┤
│   [ICON]     23.5°C                 │
│            Teilweise bewölkt        │
├─────────────────────────────────────┤
│  Feucht.    Gefühlt   Luftdruck    │
│   65%       24.2°C    1013 hPa     │
│                                     │
│   Wind: 12 km/h      UV: 3.2       │
│       Bewölkung: 45%                │
├─────────────────────────────────────┤
│ Batterie 85%     WiFi: OK  14:30   │
└─────────────────────────────────────┘
```

## 🔧 Anpassungen

### Zeitzone ändern

In [m5stack-papers3-weather.yaml](m5stack-papers3-weather.yaml) Zeile ~176:

```yaml
timezone: Europe/Berlin  # ← Deine Zeitzone
```

### Update-Intervall

Zeile ~234:

```yaml
interval: 6h  # ← z.B. 1h, 30min, 15min
```

### Deep Sleep (Batterie-Sparmodus)

Am Ende der YAML-Datei hinzufügen:

```yaml
deep_sleep:
  run_duration: 10s
  sleep_duration: 30min
```

**Batterie-Laufzeit:**
- Normal (6h Updates): ~2-3 Tage
- Deep Sleep (30min): ~2-3 Wochen
- Deep Sleep (1h): ~4-6 Wochen

## 📚 Dokumentation

📖 **Detaillierte Anleitungen:**

1. **[OPENWEATHERMAP_SETUP.md](OPENWEATHERMAP_SETUP.md)** ⭐
   - Speziell für deine OpenWeatherMap Integration
   - Alle verfügbaren Sensoren erklärt
   - Erweiterte Konfigurationen

2. **[WEATHER_QUICKSTART.md](WEATHER_QUICKSTART.md)**
   - 5-Minuten Schnellstart
   - Copy & Paste Commands
   - Häufigste Anpassungen

3. **[WEATHER_DISPLAY_GUIDE.md](WEATHER_DISPLAY_GUIDE.md)**
   - Vollständige Anleitung
   - Custom Icons
   - Home Assistant Automationen
   - Fehlerbehebung

## 🏠 Home Assistant Integration

### Service: Display aktualisieren

```yaml
service: esphome.m5papers3_weather_update_display
```

### Service: Ton abspielen

```yaml
service: esphome.m5papers3_weather_play_tone
data:
  rtttl_string: "beep:d=4,o=5,b=100:16e6"
```

### Automation: Bei Wetteränderung

```yaml
automation:
  - alias: M5Paper Weather Update
    trigger:
      platform: state
      entity_id: sensor.openweathermap_condition
    action:
      service: esphome.m5papers3_weather_update_display
```

## 🎯 Features

- ✅ **Automatische Icon-Auswahl** basierend auf Wetterbedingung
- ✅ **Echtzeit-Updates** von Home Assistant
- ✅ **Touch-Steuerung** (Tippen = Aktualisieren)
- ✅ **Batterie-Anzeige** mit Prozent & Ladestatus
- ✅ **RTC-Synchronisation** (Zeit läuft auch ohne WiFi)
- ✅ **WiFi Power-Save** für längere Akkulaufzeit
- ✅ **Services** für Home Assistant Integration
- ✅ **4 Schriftgrößen** für optimale Lesbarkeit

## 🐛 Probleme?

**Display zeigt nichts:**
```bash
esphome logs m5stack-papers3-weather.yaml
```

**Icons fehlen:**
```bash
cd /c/Users/btrom/source/repos/epdiy/scripts
python download_weather_icons.py
copy ..\weather_icons.h ..\..\esphome_components\
```

**Keine Verbindung zu HA:**
- Prüfe `secrets.yaml`
- WiFi-Status im Log checken

**Vollständige Fehlerbehebung:** Siehe [OPENWEATHERMAP_SETUP.md](OPENWEATHERMAP_SETUP.md)

## 💡 Tipps

1. **Teste zuerst** mit Standard-Einstellungen
2. **Update-Intervall** auf 6h lassen (schont Batterie & Display)
3. **Deep Sleep** nur aktivieren wenn alles funktioniert
4. **Vorhersage** kann später hinzugefügt werden
5. **Custom Icons** sind optional

## 🚀 Next Steps

Nach dem Setup kannst du erweitern mit:

- [ ] 3-Tages-Vorhersage
- [ ] Temperatur-Graph
- [ ] Unwetter-Warnungen
- [ ] Mehrere Standorte
- [ ] Custom Icons/Logos
- [ ] Touch-Menü

## 📖 Zusätzliche Ressourcen

- [EPDiy GitHub](https://github.com/vroland/epdiy)
- [ESPHome Dokumentation](https://esphome.io/)
- [M5Stack PaperS3](https://docs.m5stack.com/en/core/PaperS3)
- [OpenWeatherMap Integration](https://www.home-assistant.io/integrations/openweathermap/)

## 📝 Lizenz

MIT License - Frei verwendbar für private und kommerzielle Zwecke.

---

**Erstellt mit:** EPDiy + ESPHome + Home Assistant + OpenWeatherMap

**Hardware:** M5Stack PaperS3 (ESP32-S3, 4.7" E-Ink)

**Viel Spaß mit deinem Wetter-Display!** 🌤️
