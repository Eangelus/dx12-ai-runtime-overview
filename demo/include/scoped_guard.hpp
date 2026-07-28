#pragma once

// Generisches RAII-Guard-Muster: ruft begin() im Konstruktor auf, end()
// garantiert im Destruktor -- unabhaengig davon, ob dazwischen eine Exception
// geworfen wird.
//
// Eigenstaendige, verallgemeinerte Neuimplementierung des Musters aus
// dx12_runtime's DxDevice::FrameScope (siehe ../DEEP-DIVE.md), hier auf eine
// beliebige Resource mit begin()/end()-Methoden generalisiert, ohne
// DirectX12- oder GPU-Abhaengigkeit -- damit auf jedem Standard-CI-Runner
// kompilier- und testbar.

template <typename Resource>
class ScopedGuard {
public:
    explicit ScopedGuard(Resource& resource) : resource_(resource) {
        resource_.begin();
        began_ = true;
    }

    ~ScopedGuard() {
        if (began_) {
            resource_.end();
        }
    }

    ScopedGuard(const ScopedGuard&) = delete;
    ScopedGuard& operator=(const ScopedGuard&) = delete;

private:
    Resource& resource_;
    bool began_ = false;
};
