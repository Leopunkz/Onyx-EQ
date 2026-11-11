# Onyx EQ — GitHub Actions macOS build

Questo progetto compila automaticamente VST3 e Audio Unit universali (Apple Silicon + Intel) tramite GitHub Actions.

## Avvio senza Terminale
1. Crea un repository GitHub nuovo e vuoto.
2. Carica tutti i file e le cartelle di questo progetto nel repository, mantenendo anche la cartella nascosta `.github/workflows`.
3. Apri la scheda **Actions** del repository.
4. Se richiesto, abilita GitHub Actions.
5. Apri **Build Onyx EQ for macOS** e premi **Run workflow** (oppure attendi il completamento del workflow partito al caricamento).
6. Quando il lavoro diventa verde, apri la sua esecuzione e scarica l'artifact **Onyx-EQ-macOS-VST3-AU**.

## Installazione
Estrai lo ZIP scaricato. Copia:
- `Onyx EQ.vst3` in `~/Library/Audio/Plug-Ins/VST3/`
- `Onyx EQ.component` in `~/Library/Audio/Plug-Ins/Components/`

Riavvia Reaper e cerca **Onyx EQ** nella lista FX.

## Nota
La prima compilazione potrebbe richiedere alcuni minuti perché scarica JUCE. Se la build fallisce, apri il job e copia qui le righe finali dell'errore.

L'EQ è una ricostruzione digitale funzionale dei parametri pubblicati: ±15 dB, shelf a 80 Hz e 12 kHz, campane sweepabili 100 Hz–2 kHz e 400 Hz–8 kHz. Q=0,80 è una scelta approssimativa, non una specifica Mackie pubblicata.
