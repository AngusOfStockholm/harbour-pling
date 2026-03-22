# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# CORE RULE:
# Every piece of logic has one clear owner. No duplication.
#
# LAYERS:
# - UI: Presentation only
# - App: Orchestration / Coordination
# - Logic: Business rules / Calculations
# - Integration: Device / SFOS APIs (Sailjail compliant)
# - Persistence: Storage / Serialization
#
# RULES:
# - Simplicity: Prefer explicit, readable code over abstractions
# - Stability: Preserve behavior; modify over rewrite; no silent failures
# - Platform: Design for failure/limits; background tasks are unreliable
# - IDE: New files must be added in a project-aware, build-aware way (Qt Creator)
#
# MANDATORY DRY-RUN (Pre-Code):
# - Visibility: Seen (pasted) vs Inferred (not seen)
# - Size: Micro / Scoped / Structural
# - Placement: File path + exact anchor point
# - Layers: Affected layer(s)
#
# FILE & CONTEXT RULE:
# - Always name the file when referring to code
# - No floating code — all code must have explicit placement
# - Provide enough surrounding context to make changes unambiguous
# - If code is not pasted, it is not known — do not assume or invent structure
# - If unsure, ask instead of guessing

# The name of your application
TARGET = harbour-pling

CONFIG += sailfishapp

SOURCES += src/harbour-pling.cpp

DISTFILES += qml/harbour-pling.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/harbour-pling.changes.in \
    rpm/harbour-pling.changes.run.in \
    rpm/harbour-pling.spec \
    translations/*.ts \
    harbour-pling.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-pling-de.ts
