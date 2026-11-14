// xbdm.dll: the Xbox debugger support library.
//
// A debug build imports a few functions from it (Halo 2's debug XBE imports
// six). The retail XDK links a static copy of the library, which answers
// "there is no debugger here" for all of them, and the game code expects
// those answers: the walk calls end with a sentinel and the name call says
// there is no name, so the title keeps to itself.
#include "kernel/kernel.h"

namespace {

// the walk calls hand back this when the list is done
constexpr uint32_t kWalkEnd = 0x82DB0104;
// and the name call hands back this when there is no name to give
constexpr uint32_t kNoName = 0x2DB0000;

// a library export: the body returns eax, the wrapper pops the arguments
#define XBDM_EXPORT(library, ord, ident, nargs)                                                     \
    static uint32_t xbdm_impl_##ident(X86Context& ctx, const ::kernel::Args& a);                     \
    static void xbdm_entry_##ident(X86Context& ctx, uint8_t*) {                                      \
        ::kernel::Args args{ctx};                                                                    \
        uint32_t r = xbdm_impl_##ident(ctx, args);                                                  \
        ::kernel::Return(ctx, r, nargs);                                                             \
    }                                                                                                \
    static const bool xbdm_reg_##ident =                                                            \
        (::kernel::RegisterLibraryExport(library, ord, #ident, xbdm_entry_##ident), true);            \
    static uint32_t xbdm_impl_##ident([[maybe_unused]] X86Context& ctx, [[maybe_unused]] const ::kernel::Args& a)

}  // namespace

// DmWalkLoadedModules(context, module): the next loaded module, or the end
// of the list. There are no modules to walk without a debugger.
XBDM_EXPORT("xbdm.dll", 0x33, DmWalkLoadedModules, 2) { return kWalkEnd; }

// DmWalkModuleSections(context, module, sections): the same for the sections
// of one module.
XBDM_EXPORT("xbdm.dll", 0x34, DmWalkModuleSections, 3) { return kWalkEnd; }

// DmCloseLoadedModules(context): closes what the walk handed out.
XBDM_EXPORT("xbdm.dll", 4, DmCloseLoadedModules, 1) { return 0; }

// DmCloseModuleSections(context): closes what the section walk handed out.
XBDM_EXPORT("xbdm.dll", 5, DmCloseModuleSections, 1) { return 0; }

// DmIsDebuggerPresent(): no debugger is attached to this host.
XBDM_EXPORT("xbdm.dll", 0x15, DmIsDebuggerPresent, 0) { return 0; }

// DmGetXboxName(name, size): the console's name, which no debugger has to
// give us.
XBDM_EXPORT("xbdm.dll", 0x11, DmGetXboxName, 2) { return kNoName; }
