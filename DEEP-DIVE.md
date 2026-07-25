# Deep Dive: Exception-sicherer Frame-Lifecycle

Ein wiederkehrendes Problem bei Low-Level-GPU-Programmierung: Zwischen `BeginFrame()`
(Command-List öffnen) und `EndFrame()` (Command-List schließen und an die Queue
übergeben) kann eine Exception auftreten — z. B. wenn ein Descriptor-Pool seine
Kapazität überschreitet. Ohne Gegenmaßnahme bleibt die Command-List in diesem Fall
offen ("Reset ohne Close"), was den nächsten `BeginFrame()`-Aufruf inkonsistent macht,
weil der zugehörige Command-Allocator nicht zurückgesetzt werden darf, solange seine
Command-List noch offen ist.

Die Lösung ist ein klassisches RAII-Guard-Objekt, das den Frame-Abschluss im
Destruktor garantiert — unabhängig davon, ob der Frame regulär endet oder eine
Exception den Stack abwickelt:

```cpp
// Auszug aus src/device/DxDevice.h

// RAII: BeginFrame() im Konstruktor, EndFrame() garantiert im Destruktor —
// auch wenn zwischen Konstruktion und Zerstörung eine Exception geworfen wird
// (z.B. DescriptorPool::Alloc() bei Kapazitätsüberschreitung). Ohne diesen Guard
// bleibt die CommandList offen (Reset ohne Close), was den naechsten
// BeginFrame()-Aufruf (Allocator-Reset bei noch offener Liste) inkonsistent macht.
class FrameScope
{
public:
    explicit FrameScope(DxDevice& dev) : m_dev(dev) { m_dev.BeginFrame(); }
    ~FrameScope() { m_dev.EndFrame(); }
    FrameScope(const FrameScope&)            = delete;
    FrameScope& operator=(const FrameScope&) = delete;
private:
    DxDevice& m_dev;
};
```

Nutzung an der Aufrufstelle:

```cpp
{
    DxDevice::FrameScope frame(device);   // BeginFrame()
    // ... Dispatch-Aufrufe, Descriptor-Allokation, etc.
}                                          // EndFrame() garantiert, auch bei throw
```

**Warum das zählt:** Der Guard macht einen ganzen Fehlerklasse strukturell
unmöglich, statt sie per Try/Finally an jeder Aufrufstelle neu abzusichern —
klassisches "make invalid states unrepresentable" statt defensiver Einzelfall-Checks.
Kopieren ist bewusst per `= delete` unterbunden, damit nicht versehentlich zwei
Guards denselben Frame beenden.

---

*Teil der [DX12 AI Runtime](README.md) Projektübersicht — Implementierungsdetails
darüber hinaus (Shader, Speicherlayout, Verfahren) sind bewusst nicht Teil dieses
Repos.*
