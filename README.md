# DX12 AI Runtime

> **Showroom-Repository.** Dies ist eine kuratierte Projektübersicht ohne Quellcode
> (Shader, interne Datenformate und Verfahrensdetails sind bewusst nicht Teil dieser
> Übersicht). Der vollständige Code liegt in einem privaten Repository.

**KI-Inferenz, die in Ihrer Windows-Anwendung bleibt — ohne Python, ohne CUDA, ohne Cloud.**

DX12 AI Runtime führt den Forward-Pass eines Transformers vollständig als HLSL Compute
Shader auf DirectX 12 aus — der GPU-Schnittstelle, die auf jedem Windows-PC bereits
vorhanden ist. Aufrufbar aus jeder Win32-Anwendung über eine schlanke C-ABI-DLL: ein
Funktionsaufruf, kein Prozess, kein Port, kein Daemon.

## Kennzahlen (verifiziert)

| Kennzahl | Wert | Kontext |
|---|---|---|
| p99-Latenz | **5.93 ms** | GPT-2-Klasse: 12 Layer, dModel=768, echtes Multi-Head Attention |
| Numerische Abweichung | **0.000000** | MaxAbsError, bit-exakt vs. CPU-Referenzimplementierung |
| Langzeit-Stabilität | **~318.000 Forward-Passes** | Gestaffelter Soak-Test, 0 Fehler, 0 VRAM-Leck |
| Thermische Reserve | **62 °C max** | 28 °C Abstand zum Throttle-Punkt der Test-GPU |
| Laufzeit-Abhängigkeiten | **0** | Kein Python, kein CUDA, kein Cloud-Daemon für die Inferenz selbst |

*Referenzmessungen: NVIDIA GeForce RTX 4070, Windows 11 Pro. Gilt für die getestete
Hardware-/Treiber-/OS-Kombination.*

## Warum GPU-Inferenz auf Windows heute unbequem ist

- **CUDA/TensorRT** bindet an einen GPU-Hersteller — AMD- und Intel-Kunden bleiben außen vor.
- **ONNX Runtime + Python** verlangt eine Interpreter-Laufzeit, die in ERP-, CAD- und
  MES-Produktivumgebungen häufig schlicht nicht erlaubt ist.
- **Cloud-APIs** kosten Latenz, laufende Gebühren und werfen in regulierten Branchen
  Datenschutzfragen auf.

DX12 AI Runtime nutzt stattdessen die DirectX-12-Compute-Pipeline direkt — kein
Zusatztreiber, kein SDK-Download, kein Python-Interpreter im Produktivbetrieb.

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

## Ehrlicher Stand

**Heute verifiziert:**
- Stabile Win32-C-API (DLL-Export + Python-Wrapper, gegen echte DLL getestet)
- Bit-exakte Korrektheit des kompletten Transformer-Blocks inkl. echtem Multi-Head Attention
- Dauerlast geprüft: kein VRAM-Leck, keine Fehler über ~318.000 Aufrufe
- Durchgängig verifiziert auf Windows 11 + NVIDIA

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
