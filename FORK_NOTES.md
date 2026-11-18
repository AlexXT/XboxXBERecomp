# Notes on this fork

## Lineage and purpose

Forked from [hk21x/XboxXBERecomp](https://github.com/hk21x/XboxXBERecomp),
which is itself a fork of
[TGP482/XboxXBERecomp](https://github.com/TGP482/XboxXBERecomp).

Most of the work on this branch was done while bringing up the **Halo 2 May
2003 beta** — the `halo2debug.xbe` build in particular. It carries the whole
pstencil (stencil shadow volume) engine that retail dropped, a linker map
that names its functions, and the XDK **5455** build, none of which upstream
covers. The game-side project is
[AlexXT/halo2xbe](https://github.com/AlexXT/halo2xbe).

The commits below are grouped by what drove them. Several are general
improvements found while chasing a Halo 2 symptom; those are useful to any
title. The branch is 24 commits ahead of `hk21x/main` (including the merge
that brought the controller-stick work in), and the merge base is `60fc186`.

## Halo 2 beta bring-up

| Commit | What it fixes |
|---|---|
| `2adb193`, `0b44219` | Port and add the `xbdm.dll` host and **XDK 5455 signatures**. The beta is a 5455 build; neither the original fork nor upstream shipped its signatures, so `xbe_symbols` could not name the library at all. (The upstream `xdk/5849` set is untouched and still there.) |
| `2ef90e6` | Development kits had 128 MB of physical memory. The debug build asks for ~53 MB in one contiguous block, which does not fit the fork's 64 MB guest pool. The title now gets the whole kit-sized pool and the host keeps its own buffers. |
| `a9bd860` | The 5455 texture-stage-state stride is 32 dwords, and the pixel shader the **device** holds is the one the draws sample. Together these are what make Halo 2's world textured instead of flat. |
| `c5cbc49` | Pushed simple render states now reach the state that draws actually use, so a title whose own D3D initialises through the push buffer draws its colour. |
| `2500d93` | A title with no `D3D_InitDefaultState` gets console defaults from the host; identity gamma ramp at start, `D3DTOP_DISABLE` stages genuinely off, and 5455's own render-state numbering in the tables. |
| `282d3c2` | The back-buffer scale is not at `0x15F0` in this build — the host was writing 1.0 over the back-buffer pointer. The layout now names the front buffer, back buffer and the render target the library code reads, and writes all of them. |
| `4eae1ad` | The vertical-blank callback sits at a different offset in 5455; use the offset this XDK actually writes. |
| `e81f50d` | Skip debug asserts with a balanced guest stack, and clear map caches on every build. |
| `6561182` | Remove the duplicate `SetDepthClipPlanes` stub; keep the one the library exports. |
| `c1a3d76` | In-game developer console bridge (the beta's console is part of what makes its symbols and pixel stage observable). |

## General fixes found on the way

| Commit | What it fixes |
|---|---|
| `76ebd17` | A `[names]` config entry gives generated code the game's own function names instead of `sub_%08X`. Upstream had it; the original fork did not. |
| `fecb7db`, `cc510ae`, `5c6c3f5` | Callbacks (immediates pointing at code) are found before pointer seeds and gap fill, and are entries even where `PlausibleEntry` says no. An immediate operand is the guest saying "this is code", not something to filter. |
| `13ad80d` | A fault report says whether the guest address was committed and shows the guest/host shared stack, so a fault in host code can be told from one in the guest. |
| `ea67ffd` | A title that stops drawing names its threads and stacks, so a wait that never ends is visible. |
| `d518cf8` | A missing included config is a warning; a config that includes itself is an error instead of an endless read. |
| `cf2b665` | Every build starts without shader and pipeline caches — a stale pipeline from another build draws the wrong thing or crashes. |
| `8cc40dd` | Use SDL's XInput backend so Xbox pads are detected on Windows. |
| `fa0e050`, `ec0213b`, `b908ae7` | Controller-stick dead zone and sensitivity per axis, tuned in `Poll` from the read inputs and applied past the game's own dead zone. (Follows a NightFire issue; merged in via `91f685d`.) |

## What is not here

No game assets, no XBE, no generated code, no keys. The `5849` XDK signatures
came from the upstream fork; this branch **adds** `5455`, it does not replace
anything. The toolkit's own licence, `NOTICE` and `THIRD_PARTY.txt` are
unchanged.

For the game side — configs, naming tools, the pstencil findings and the
build/run notes — see [AlexXT/halo2xbe](https://github.com/AlexXT/halo2xbe).
