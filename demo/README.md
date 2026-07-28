# Demo: Exception-sicherer Resource-Guard

[![CI](https://github.com/Eangelus/dx12-ai-runtime-overview/actions/workflows/ci.yml/badge.svg)](https://github.com/Eangelus/dx12-ai-runtime-overview/actions/workflows/ci.yml)

Eigenständige, verallgemeinerte Neuimplementierung des RAII-Frame-Lifecycle-
Musters aus [DX12 AI Runtime](../README.md) (siehe [../DEEP-DIVE.md](../DEEP-DIVE.md)):
`ScopedGuard<Resource>` ruft `begin()` im Konstruktor und garantiert `end()`
im Destruktor — unabhängig davon, ob dazwischen eine Exception geworfen wird.

**Kein Auszug aus dem produktiven Code.** Das echte `DxDevice::FrameScope` ist
fest an DirectX 12 gebunden; dieses Demo generalisiert dasselbe Muster auf eine
beliebige Resource mit `begin()`/`end()`, ohne GPU- oder DirectX12-Abhängigkeit
— damit es auf jedem Standard-CI-Runner ohne GPU kompiliert und getestet
werden kann (das echte Projekt braucht dafür einen self-hosted Windows/GPU-
Runner, siehe Hauptrepo).

## Komponenten

| Datei | Rolle |
|---|---|
| `include/scoped_guard.hpp` | `ScopedGuard<Resource>` — generisches RAII-Guard-Template |
| `include/mock_device.hpp` | `MockDevice` — simuliertes Gerät für Tests, zählt `begin()`/`end()`-Aufrufe |
| `tests/test_scoped_guard.cpp` | 4 Tests: regulärer Ablauf, Exception mitten im Scope, verschachtelte Scopes, sequentielle Wiederverwendung |

## Beispiel

```cpp
MockDevice dev;
try {
    ScopedGuard<MockDevice> guard(dev);   // ruft dev.begin()
    // ... Arbeit, die eine Exception werfen kann ...
    throw std::runtime_error("etwas ging schief");
} catch (const std::runtime_error&) {
    // guard's Destruktor hat bereits dev.end() aufgerufen --
    // dev.is_open() == false, auch nach der Exception.
}
```

## Build & Tests

```bash
cd demo
g++ -std=c++17 -Wall -Wextra -Werror -Iinclude tests/test_scoped_guard.cpp -o test_scoped_guard
./test_scoped_guard
```

Kompiliert warnungsfrei, 4 Tests grün. Läuft bei jedem Push über GitHub Actions
(Badge oben).
