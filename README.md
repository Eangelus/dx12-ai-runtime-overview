# DX12 AI Runtime

> Kuratierte Projektübersicht ohne Quellcode (Shader, interne Datenformate und
> Verfahrensdetails sind bewusst nicht Teil dieser Übersicht). Der vollständige Code
> liegt in einem privaten Repository.

**KI-Inferenz, die in Ihrer Windows-Anwendung bleibt — ohne Python, ohne CUDA, ohne Cloud.**

DX12 AI Runtime führt den Forward-Pass eines Transformers vollständig als HLSL Compute
Shader auf DirectX 12 aus — der GPU-Schnittstelle, die auf jedem Windows-PC bereits
vorhanden ist. Aufrufbar aus jeder Win32-Anwendung über eine schlanke C-ABI-DLL: ein
Funktionsaufruf, kein Prozess, kein Port, kein Daemon.

## Kennzahlen (verifiziert)

| Kennzahl | Wert | Kontext |
|---|---|---|
| p99-Latenz | **5.93 ms** | GPT-2-Klasse: 12 Layer, dModel=768, 12 Attention-Köpfe à 64 Dim, seqLen=16, Batch=1 |
| Numerische Abweichung (intern) | **0.000000** | MaxAbsError ggü. eigener CPU-Referenzimplementierung¹ |
| Numerische Abweichung (extern) | **100% innerhalb Toleranz** | ggü. ONNX Runtime CPU², atol=2.5e-3 + rtol=1e-2 |
| Langzeit-Stabilität | **~318.000 Forward-Passes** | Gestaffelter Soak-Test (60s→900s), 0 Fehler, 0 VRAM-Leck |
| Thermische Reserve | **62 °C max** | RTX 4070, 28 °C Abstand zum Throttle-Punkt (~90 °C) |
| Laufzeit-Abhängigkeiten | **0** | Kein Python, kein CUDA, kein Cloud-Daemon für die Inferenz selbst |

*Referenzmessungen: NVIDIA GeForce RTX 4070 12 GB, Windows 11 Pro, FP32 durchgängig.
Gilt für die getestete Hardware-/Treiber-/OS-Kombination — noch keine AMD/Intel-Messung
(siehe „Als Nächstes" unten).*

¹ *„Bit-exakt" bezieht sich konkret auf: identischer HLSL-Compute-Output vs. eine im
selben Repo mitgeführte, unabhängig implementierte, naive CPU-Referenzberechnung
derselben Operation (z. B. MatMul, Attention) — gleiche Eingabetensoren, gleiche
FP32-Präzision, deterministisch (kein Sampling). Das ist ein interner
Korrektheits-Check des GPU-Pfads gegen die eigene Spezifikation, kein Vergleich
gegen ein externes Framework.*

² *Der Vergleich gegen ONNX Runtime CPU (unabhängige, weit verbreitete Referenz-
Implementierung) ist bewusst mit einer lockereren, für Float-Vergleiche über mehrere
Layer üblichen Toleranz bewertet (kombiniert absolut+relativ), nicht bit-exakt — beide
Prüfungen zusammen (strikt intern + toleranzbasiert extern) ergeben das Gesamtbild.*

## Warum GPU-Inferenz auf Windows heute unbequem ist

- **CUDA/TensorRT** bindet an einen GPU-Hersteller — AMD- und Intel-Kunden bleiben außen vor.
- **ONNX Runtime + Python** verlangt eine Interpreter-Laufzeit, die in ERP-, CAD- und
  MES-Produktivumgebungen häufig schlicht nicht erlaubt ist.
- **Cloud-APIs** kosten Latenz, laufende Gebühren und werfen in regulierten Branchen
  Datenschutzfragen auf.

DX12 AI Runtime nutzt stattdessen die DirectX-12-Compute-Pipeline direkt — die
DirectX-12-fähige GPU samt Standardtreiber wird weiterhin vorausgesetzt (wie bei
jeder Windows-Anwendung mit Grafik-/Compute-Bezug), aber **zusätzlich** zu diesem
ohnehin auf jedem Windows-PC vorhandenen Treiber ist kein separates SDK, kein
CUDA-Toolkit und kein Python-Interpreter im Produktivbetrieb nötig.

## Signalweg

```
Ihre Anwendung  →  dx12_runtime.dll  →  DirectX 12 Compute  →  GPU
(Win32-Prozess)    (stabile C-ABI)      (HLSL-Kernel)          (NVIDIA verifiziert,
                                                                 AMD/Intel Arc in Vorbereitung)
```

Ein Python-ctypes-Wrapper steht zusätzlich für Data-Science- und Agenten-Stacks bereit,
die aus Python heraus aufrufen möchten.

## Wo das etwas bringt

| Segment | Nutzen |
|---|---|
| **ERP-Plugin-Entwickler** | KI-gestützte Dateneingabe/Anomalie-Erkennung ohne Python in der Produktionsumgebung |
| **CAD-Add-ins** | Geometrie-Suche/Feature-Extraktion, läuft auch auf AMD-Workstations, nicht nur CUDA |
| **MES/Fertigung** | Predictive Quality in air-gapped Werkshallen, ohne Internetanbindung |
| **Windows-Kiosksysteme** | Kaltstart ohne Daemon — DLL laden und in Millisekunden inferieren |

## Code-Beispiel

→ [DEEP-DIVE.md](DEEP-DIVE.md) — ein konkreter Architektur-Ausschnitt (exception-sicherer
Frame-Lifecycle per RAII) mit Code und Begründung, statt nur Fließtext.

→ [demo/](demo/) — eine eigenständige, kompilier- und testbare Mini-Demo desselben
RAII-Musters (generalisiert, ohne GPU-Abhängigkeit), mit Tests und CI-Pipeline.
Kein Auszug aus dem produktiven Code.

## Ehrlicher Stand

**Heute verifiziert:**
- Stabile Win32-C-API (DLL-Export + Python-Wrapper, gegen echte DLL getestet)
- Korrektheit des kompletten Transformer-Blocks inkl. echtem Multi-Head Attention:
  bit-exakt gegen die eigene CPU-Referenz, innerhalb Toleranz gegen ONNX Runtime CPU
  (Details siehe Kennzahlen-Fußnoten oben)
- Dauerlast geprüft: kein VRAM-Leck, keine Fehler über ~318.000 Aufrufe
- Durchgängig verifiziert auf Windows 11 + NVIDIA — AMD/Intel noch offen

**Als Nächstes:**
- AMD- und Intel-Arc-Validierung (Hardware-Matrix wird bewusst erweitert)
- Höherer GPU-Durchsatz (Tiled GEMM, asynchrones Command-Submit)
- Autoregressive Textgenerierung (erfordert KV-Cache, aktuell nicht enthalten)

## Lizenzierung

Kommerzielles Lizenzmodell, Details auf Anfrage.

## Kontakt

**Thomas Bernecker** — Entwicklung & Benchmarks
[bernecker.thomas@gmx.de](mailto:bernecker.thomas@gmx.de)

---

*Dieses Repository dient ausschließlich der Projektvorstellung. Es enthält bewusst keinen
Quellcode, keine Shader, keine internen Datenformate und keine Verfahrensdetails.*
