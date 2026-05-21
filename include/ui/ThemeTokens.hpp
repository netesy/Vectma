#pragma once

#include "core/GColor.hpp"

namespace vectma {
namespace tokens {

// Colors
namespace colors {
    static const GColor Surface = GColor::FromHex("#131313");
    static const GColor SurfaceDim = GColor::FromHex("#131313");
    static const GColor SurfaceBright = GColor::FromHex("#393939");
    static const GColor SurfaceContainerLowest = GColor::FromHex("#0e0e0e");
    static const GColor SurfaceContainerLow = GColor::FromHex("#1b1b1c");
    static const GColor SurfaceContainer = GColor::FromHex("#202020");
    static const GColor SurfaceContainerHigh = GColor::FromHex("#2a2a2a");
    static const GColor SurfaceContainerHighest = GColor::FromHex("#353535");
    static const GColor OnSurface = GColor::FromHex("#e5e2e1");
    static const GColor OnSurfaceVariant = GColor::FromHex("#cfc2d6");
    static const GColor InverseSurface = GColor::FromHex("#e5e2e1");
    static const GColor InverseOnSurface = GColor::FromHex("#303030");
    static const GColor Outline = GColor::FromHex("#988d9f");
    static const GColor OutlineVariant = GColor::FromHex("#4d4354");
    static const GColor SurfaceTint = GColor::FromHex("#ddb7ff");
    static const GColor Primary = GColor::FromHex("#ddb7ff");
    static const GColor OnPrimary = GColor::FromHex("#490080");
    static const GColor PrimaryContainer = GColor::FromHex("#b76dff");
    static const GColor OnPrimaryContainer = GColor::FromHex("#400071");
    static const GColor InversePrimary = GColor::FromHex("#842bd2");
    static const GColor Secondary = GColor::FromHex("#5de6ff");
    static const GColor OnSecondary = GColor::FromHex("#00363e");
    static const GColor SecondaryContainer = GColor::FromHex("#00cbe6");
    static const GColor OnSecondaryContainer = GColor::FromHex("#00515d");
    static const GColor Tertiary = GColor::FromHex("#fabc4e");
    static const GColor OnTertiary = GColor::FromHex("#432c00");
    static const GColor TertiaryContainer = GColor::FromHex("#bd871a");
    static const GColor OnTertiaryContainer = GColor::FromHex("#3a2600");
    static const GColor Error = GColor::FromHex("#ffb4ab");
    static const GColor OnError = GColor::FromHex("#690005");
    static const GColor ErrorContainer = GColor::FromHex("#93000a");
    static const GColor OnErrorContainer = GColor::FromHex("#ffdad6");
    static const GColor PrimaryFixed = GColor::FromHex("#f0dbff");
    static const GColor PrimaryFixedDim = GColor::FromHex("#ddb7ff");
    static const GColor OnPrimaryFixed = GColor::FromHex("#2c0051");
    static const GColor OnPrimaryFixedVariant = GColor::FromHex("#6900b3");
    static const GColor SecondaryFixed = GColor::FromHex("#a2eeff");
    static const GColor SecondaryFixedDim = GColor::FromHex("#2fd9f4");
    static const GColor OnSecondaryFixed = GColor::FromHex("#001f25");
    static const GColor OnSecondaryFixedVariant = GColor::FromHex("#004e5a");
    static const GColor TertiaryFixed = GColor::FromHex("#ffdead");
    static const GColor TertiaryFixedDim = GColor::FromHex("#fabc4e");
    static const GColor OnTertiaryFixed = GColor::FromHex("#281900");
    static const GColor OnTertiaryFixedVariant = GColor::FromHex("#604100");
    static const GColor Background = GColor::FromHex("#131313");
    static const GColor OnBackground = GColor::FromHex("#e5e2e1");
    static const GColor SurfaceVariant = GColor::FromHex("#353535");
}

// Spacing
namespace spacing {
    static constexpr float SidebarWidth = 240.0f;
    static constexpr float ToolbarWidth = 48.0f;
    static constexpr float ToolbarHeight = 48.0f;
    static constexpr float StatusBarHeight = 24.0f;
    static constexpr float GutterCompact = 4.0f;
    static constexpr float PanelPadding = 12.0f;
    static constexpr float ControlGap = 8.0f;
}

// Typography
namespace typography {
    struct FontScale {
        float size;
        int weight;
        float lineHeight;
        float letterSpacing;
    };

    static const FontScale PanelHeader = { 11.0f, 600, 16.0f, 0.05f };
    static const FontScale BodyMd = { 13.0f, 400, 18.0f, 0.0f };
    static const FontScale LabelSm = { 11.0f, 500, 14.0f, 0.0f };
    static const FontScale ValueMono = { 12.0f, 400, 16.0f, 0.0f };
    static const FontScale HeadlineSm = { 18.0f, 600, 24.0f, 0.0f };
}

} // namespace tokens
} // namespace vectma
