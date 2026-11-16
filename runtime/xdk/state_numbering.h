#pragma once
// Render state (D3DRS) and texture stage state (D3DTSS) numbers of an XDK generation. The runtime keeps 5659's
// (gpu::kRs*, gpu::kTss*) and translates where it reads or writes the title's own tables; layout.toml's
// StateNumbering picks the generation, 5659 when absent. Simple render states need no table: they are keyed by the
// method the title's D3D_SimpleRenderStateEncode gives each one.
#include <array>
#include <cstdint>

#include "gpu/state.h"
#include "image_symbols.h"

#ifndef XDK_StateNumbering
#define XDK_StateNumbering 5659
#endif
static_assert(XDK_StateNumbering == 5659 || XDK_StateNumbering == 3948 || XDK_StateNumbering == 4039 ||
                  XDK_StateNumbering == 5455,
              "layout.toml StateNumbering: 5659, 5455, 4039 or 3948 (a new generation needs its table in xdk/state_numbering.h)");

namespace xdk::states {
constexpr uint32_t kNone = 0xFFFFFFFFu;

struct Numbering {
    uint32_t deferred_first;                   // the title's first deferred render state, lower ones are simple
    uint32_t count;                            // the title's render state count
    std::array<uint32_t, gpu::kRsMax> rs;      // 5659's index -> the title's, kNone where the title has no such state
    std::array<uint32_t, gpu::kTssCount> tss;  // the same for texture stage states
};

constexpr Numbering Make5659() {
    Numbering n{gpu::kRsDeferredFirst, gpu::kRsMax, {}, {}};
    for (uint32_t i = 0; i < gpu::kRsMax; i++) n.rs[i] = i;
    for (uint32_t i = 0; i < gpu::kTssCount; i++) n.tss[i] = i;
    return n;
}

// 3948 (the launch XDK): 82 simple, 34 deferred and 28 complex states. Deferred are 5659's FogEnable to
// PatchSegments, 10 lower (no SwapFilter, PresentationInterval or unused slots). Complex lack 5659's
// MultiSampleMode, MultiSampleRenderTargetMode (3948's MultiSampleType at 134 sits between) and SampleAlpha.
// Texture stage states put the combiner states (ColorOp to TextureTransformFlags) first, then AddressU to AlphaKill.
constexpr Numbering Make3948() {
    Numbering n{82, 144, {}, {}};
    for (auto& r : n.rs) r = kNone;
    for (uint32_t i = gpu::kRsFogEnable; i <= gpu::kRsPointSizeMax + 2; i++) n.rs[i] = i - 10;
    for (uint32_t i = gpu::kRsPsTextureModes; i <= gpu::kRsMultiSampleMask; i++) n.rs[i] = i - 20;
    n.rs[gpu::kRsShadowFunc] = 135;
    n.rs[gpu::kRsLineWidth] = 136;
    for (uint32_t i = gpu::kRsDxt1NoiseEnable; i <= gpu::kRsDoNotCullUncompressed; i++) n.rs[i] = i - 22;
    for (uint32_t i = 0; i < gpu::kTssCount; i++) n.tss[i] = i;
    for (uint32_t i = gpu::kTssAddressU; i <= gpu::kTssAlphaKill; i++) n.tss[i] = i + 10;
    for (uint32_t i = gpu::kTssColorOp; i <= gpu::kTssTextureTransformFlags; i++) n.tss[i] = i - 12;
    return n;
}

// 4039: 82 simple, 35 deferred and 29 complex states. Deferred as 3948's, with SwapFilter last (the frame buffer init
// sets it); complex are 5659's less SampleAlpha. Texture stage states as 5659's.
constexpr Numbering Make4039() {
    Numbering n{82, 146, {}, {}};
    for (auto& r : n.rs) r = kNone;
    for (uint32_t i = gpu::kRsFogEnable; i <= gpu::kRsPointSizeMax + 2; i++) n.rs[i] = i - 10;
    n.rs[gpu::kRsPresentationInterval - 1] = 116;
    for (uint32_t i = gpu::kRsPsTextureModes; i <= gpu::kRsLineWidth; i++) n.rs[i] = i - 19;
    for (uint32_t i = gpu::kRsDxt1NoiseEnable; i <= gpu::kRsDoNotCullUncompressed; i++) n.rs[i] = i - 20;
    for (uint32_t i = 0; i < gpu::kTssCount; i++) n.tss[i] = i;
    return n;
}

// 5455 (the XDK the Halo 2 beta links): 82 simple, 35 deferred and 29 complex states, the deferred ones as
// 4039's (the fog states and the lighting) and the complex ones from PsTextureModes on. The texture stage states
// are the Xbox's own list: the address modes and the filters first, the combiner states from ColorOp on.
constexpr Numbering Make5455() {
    Numbering n{82, 147, {}, {}};
    for (auto& r : n.rs) r = kNone;
    n.rs[gpu::kRsFogEnable] = 82;
    n.rs[gpu::kRsFogTableMode] = 83;
    n.rs[gpu::kRsFogStart] = 84;
    n.rs[gpu::kRsFogEnd] = 85;
    n.rs[gpu::kRsFogDensity] = 86;
    n.rs[gpu::kRsRangeFogEnable] = 87;
    n.rs[gpu::kRsWrap0] = 88;
    n.rs[gpu::kRsLighting] = 92;
    n.rs[gpu::kRsSpecularEnable] = 93;
    n.rs[gpu::kRsLocalViewer] = 94;
    n.rs[gpu::kRsColorVertex] = 95;
    n.rs[gpu::kRsSpecularMaterialSource] = 100;
    n.rs[gpu::kRsDiffuseMaterialSource] = 101;
    n.rs[gpu::kRsAmbientMaterialSource] = 102;
    n.rs[gpu::kRsEmissiveMaterialSource] = 103;
    n.rs[gpu::kRsAmbient] = 105;
    n.rs[gpu::kRsPointSize] = 106;
    n.rs[gpu::kRsPointSizeMin] = 107;
    n.rs[gpu::kRsPointSpriteEnable] = 108;
    n.rs[gpu::kRsPointScaleEnable] = 109;
    n.rs[gpu::kRsPointScaleA] = 110;
    n.rs[gpu::kRsPointScaleB] = 111;
    n.rs[gpu::kRsPointScaleC] = 112;
    n.rs[gpu::kRsPointSizeMax] = 113;
    n.rs[gpu::kRsPsTextureModes] = 117;
    n.rs[gpu::kRsVertexBlend] = 118;
    n.rs[gpu::kRsFogColor] = 119;
    n.rs[gpu::kRsFillMode] = 120;
    n.rs[gpu::kRsBackFillMode] = 121;
    n.rs[gpu::kRsTwoSidedLighting] = 122;
    n.rs[gpu::kRsNormalizeNormals] = 123;
    n.rs[gpu::kRsZEnable] = 124;
    n.rs[gpu::kRsStencilEnable] = 125;
    n.rs[gpu::kRsStencilFail] = 126;
    n.rs[gpu::kRsFrontFace] = 127;
    n.rs[gpu::kRsCullMode] = 128;
    n.rs[gpu::kRsTextureFactor] = 129;
    n.rs[gpu::kRsZBias] = 130;
    n.rs[gpu::kRsLogicOp] = 131;
    n.rs[gpu::kRsEdgeAntiAlias] = 132;
    n.rs[gpu::kRsMultiSampleAntiAlias] = 133;
    n.rs[gpu::kRsMultiSampleMask] = 134;
    n.rs[gpu::kRsMultiSampleMode] = 135;
    n.rs[gpu::kRsMultiSampleRenderTargetMode] = 136;
    n.rs[gpu::kRsShadowFunc] = 137;
    n.rs[gpu::kRsLineWidth] = 138;
    n.rs[gpu::kRsDxt1NoiseEnable] = 139;
    n.rs[gpu::kRsYuvEnable] = 140;
    n.rs[gpu::kRsOcclusionCullEnable] = 141;
    n.rs[gpu::kRsStencilCullEnable] = 142;
    n.rs[gpu::kRsRopZCmpAlwaysRead] = 143;
    n.rs[gpu::kRsRopZRead] = 144;
    n.rs[gpu::kRsDoNotCullUncompressed] = 145;
    for (auto& t : n.tss) t = kNone;
    n.tss[gpu::kTssAddressU] = 0;
    n.tss[gpu::kTssAddressV] = 1;
    n.tss[gpu::kTssAddressW] = 2;
    n.tss[gpu::kTssMagFilter] = 3;
    n.tss[gpu::kTssMinFilter] = 4;
    n.tss[gpu::kTssMipFilter] = 5;
    n.tss[gpu::kTssMipLodBias] = 6;
    n.tss[gpu::kTssMaxMipLevel] = 7;
    n.tss[gpu::kTssMaxAnisotropy] = 8;
    n.tss[gpu::kTssColorKeyOp] = 9;
    n.tss[gpu::kTssColorSign] = 10;
    n.tss[gpu::kTssAlphaKill] = 11;
    n.tss[gpu::kTssColorOp] = 12;
    n.tss[gpu::kTssColorArg0] = 13;
    n.tss[gpu::kTssColorArg1] = 14;
    n.tss[gpu::kTssColorArg2] = 15;
    n.tss[gpu::kTssAlphaOp] = 16;
    n.tss[gpu::kTssAlphaArg0] = 17;
    n.tss[gpu::kTssAlphaArg1] = 18;
    n.tss[gpu::kTssAlphaArg2] = 19;
    n.tss[gpu::kTssResultArg] = 20;
    n.tss[gpu::kTssTextureTransformFlags] = 21;
    n.tss[gpu::kTssBumpEnvMat00] = 22;
    n.tss[gpu::kTssBumpEnvMat01] = 23;
    n.tss[gpu::kTssBumpEnvMat11] = 24;
    n.tss[gpu::kTssBumpEnvMat10] = 25;
    n.tss[gpu::kTssBumpEnvLScale] = 26;
    n.tss[gpu::kTssBumpEnvLOffset] = 27;
    n.tss[gpu::kTssTexCoordIndex] = 28;
    n.tss[gpu::kTssBorderColor] = 29;
    n.tss[gpu::kTssColorKeyColor] = 30;
    return n;
}

constexpr Numbering kNumbering = XDK_StateNumbering == 3948   ? Make3948()
                                 : XDK_StateNumbering == 4039 ? Make4039()
                                 : XDK_StateNumbering == 5455 ? Make5455()
                                                               : Make5659();

// the title's render state -> 5659's, kNone for a simple state or one 5659 lacks
constexpr uint32_t RsFromTitle(uint32_t t) {
    for (uint32_t i = gpu::kRsDeferredFirst; i < gpu::kRsMax; i++)
        if (kNumbering.rs[i] == t) return i;
    return kNone;
}
constexpr uint32_t TssFromTitle(uint32_t t) {
    for (uint32_t i = 0; i < gpu::kTssCount; i++)
        if (kNumbering.tss[i] == t) return i;
    return kNone;
}
}  // namespace xdk::states
