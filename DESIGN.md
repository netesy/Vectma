Adapting the Vectma UI Engine for dual modes requires preserving that razor-sharp, surgical feel whether the user is in a pitch-black editing bay or a brightly lit laboratory space.

To maintain your **border-free** philosophy, the Light Mode inverts the tonal elevation scale—moving from crisp, sterile whites to structured architectural silvers instead of using dark charcoals. The high-performance "Focused Teal" shifts to a deeper, high-contrast teal in light mode to keep text and fine vector paths highly legible.

Here is the updated, unified specification for your design system.

---

```yaml
name: Vectma UI Engine
modes:
  dark:
    colors:
      surface: '#131314'
      surface-dim: '#131314'
      surface-bright: '#3a393a'
      surface-container-lowest: '#0e0e0f'
      surface-container-low: '#1c1b1c'
      surface-container: '#201f20'
      surface-container-high: '#2a2a2b'
      surface-container-highest: '#353436'
      on-surface: '#e5e2e3'
      on-surface-variant: '#bdc9c8'
      inverse-surface: '#e5e2e3'
      inverse-on-surface: '#313031'
      outline: '#879392'
      outline-variant: '#3e4949'
      surface-tint: '#76d6d5'
      primary: '#76d6d5'
      on-primary: '#003737'
      primary-container: '#008080'
      on-primary-container: '#e3fffe'
      inverse-primary: '#006a6a'
      secondary: '#65d8d7'
      on-secondary: '#003737'
      secondary-container: '#1aa1a1'
      on-secondary-container: '#002f2f'
      tertiary: '#c8c6c9'
      on-tertiary: '#303033'
      tertiary-container: '#737276'
      on-tertiary-container: '#fbf8fc'
      error: '#ffb4ab'
      on-error: '#690005'
      error-container: '#93000a'
      on-error-container: '#ffdad6'
      primary-fixed: '#93f2f2'
      primary-fixed-dim: '#76d6d5'
      on-primary-fixed: '#002020'
      on-primary-fixed-variant: '#004f4f'
      secondary-fixed: '#83f4f4'
      secondary-fixed-dim: '#65d8d7'
      on-secondary-fixed: '#002020'
      on-secondary-fixed-variant: '#004f4f'
      tertiary-fixed: '#e4e1e5'
      tertiary-fixed-dim: '#c8c6c9'
      on-tertiary-fixed: '#1b1b1e'
      on-tertiary-fixed-variant: '#47464a'
      background: '#131314'
      on-background: '#e5e2e3'
      surface-variant: '#353436'
  
  light:
    colors:
      surface: '#f9f9fa'
      surface-dim: '#dcd9db'
      surface-bright: '#ffffff'
      surface-container-lowest: '#ffffff'
      surface-container-low: '#f3f3f4'
      surface-container: '#ededee'
      surface-container-high: '#e8e7e9'
      surface-container-highest: '#e2e1e3'
      on-surface: '#1b1b1c'
      on-surface-variant: '#3f4948'
      inverse-surface: '#313031'
      inverse-on-surface: '#f4f0f1'
      outline: '#6f7979'
      outline-variant: '#bec8c7'
      surface-tint: '#006a6a'
      primary: '#006a6a'
      on-primary: '#ffffff'
      primary-container: '#a6f2f1'
      on-primary-container: '#002020'
      inverse-primary: '#76d6d5'
      secondary: '#006a69'
      on-secondary: '#ffffff'
      secondary-container: '#a3f3f2'
      on-secondary-container: '#002020'
      tertiary: '#5e5e62'
      on-tertiary: '#ffffff'
      tertiary-container: '#e4e1e5'
      on-tertiary-container: '#1b1b1e'
      error: '#ba1a1a'
      on-error: '#ffffff'
      error-container: '#ffdad6'
      on-error-container: '#410002'
      primary-fixed: '#93f2f2'
      primary-fixed-dim: '#76d6d5'
      on-primary-fixed: '#002020'
      on-primary-fixed-variant: '#004f4f'
      secondary-fixed: '#83f4f4'
      secondary-fixed-dim: '#65d8d7'
      on-secondary-fixed: '#002020'
      on-secondary-fixed-variant: '#004f4f'
      tertiary-fixed: '#e4e1e5'
      tertiary-fixed-dim: '#c8c6c9'
      on-tertiary-fixed: '#1b1b1e'
      on-tertiary-fixed-variant: '#47464a'
      background: '#f9f9fa'
      on-background: '#1b1b1c'
      surface-variant: '#dbe5e4'

typography:
  display-lg:
    fontFamily: Inter
    fontSize: 32px
    fontWeight: '700'
    lineHeight: 40px
    letterSpacing: -0.02em
  headline-md:
    fontFamily: Inter
    fontSize: 20px
    fontWeight: '600'
    lineHeight: 28px
    letterSpacing: -0.01em
  body-base:
    fontFamily: Inter
    fontSize: 14px
    fontWeight: '400'
    lineHeight: 20px
  body-bold:
    fontFamily: Inter
    fontSize: 14px
    fontWeight: '600'
    lineHeight: 20px
  label-caps:
    fontFamily: Inter
    fontSize: 11px
    fontWeight: '800'
    lineHeight: 14px
    letterSpacing: 0.08em
  inspector-tech:
    fontFamily: JetBrains Mono
    fontSize: 12px
    fontWeight: '400'
    lineHeight: 16px

rounded:
  sm: 0.25rem
  DEFAULT: 0.5rem
  md: 0.75rem
  lg: 1rem
  xl: 1.5rem
  full: 9999px

spacing:
  unit: 2px
  xs: 4px
  sm: 6px
  md: 8px
  lg: 10px
  xl: 12px
  xxl: 15px
  gutter: 10px
  window-padding: 15px

```

---

## Brand & Style

The design system is engineered for professional C++ vector manipulation, prioritizing high-performance immersion and technical surgicality. The brand personality is **Elite, Methodical, and Deep**, designed to vanish into the background so the user's creative output remains the focal point. It targets power users who require long-session stamina and precision.

The visual style is a hybrid of **Modern Minimalism** and **Technical Premium**. It rejects the industrial clutter of legacy CAD software by eliminating harsh borders and rigid grids in favor of soft, organic geometry and depth through tonal layering.

* **Immersive Dark:** A near-black environment minimizes ocular strain for low-light environments and late-night marathons.
* **Surgical Light:** A clean, high-contrast laboratory archetype inspired by blueprint schematics and high-end physical measurement gear. It guarantees perfect clarity in high-glare environments.
* **Immediate-Mode Aesthetic:** High-density controls that feel responsive and tactile.
* **Soft Precision:** High rounding values (12px, 10px, 8px) soften the technical "edge" without sacrificing the industrial feel.
* **Border-Free:** Hierarchy is defined entirely by color elevation and whitespace, not lines.

## Colors

The palette relies on a foundation of **Obsidian Grays** (Dark) or **Laboratory Silvers** (Light), balanced alongside an adaptive **Focused Teal** system. The primary goal is to maintain a low-noise environment where only interactive elements or active vector paths command attention.

* **Primary (Focused Teal):** Used for active states, selection headers, and primary action buttons. In Dark Mode, it uses an emissive light teal (`#76d6d5`); in Light Mode, it tightens into a deep architectural teal (`#006a6a`) to secure an accessible contrast ratio against light backgrounds.
* **Secondary (Electric Teal):** Reserved for high-motion feedback, such as active slider scrubbing or handle dragging.
* **Neutral Base:** The root environment background (`#131314` for dark; `#f9f9fa` for light).
* **Surface:** Used for floating panels and child windows to create structural elevation layers.

> **Contrast Protection:** To prevent "haloing" and visual fatigue, pure white (#FFFFFF) is forbidden for text in Dark Mode—use **Surgical Silver** instead. In Light Mode, pure black (#000000) is avoided for UI copy—use **Deep Charcoal** to prevent stark, high-frequency text vibrating against the clean surfaces.

## Elevation & Depth

Hierarchy is communicated through **Tonal Layering** and **Subtle Translucency**. This system avoids heavy drop shadows in favor of surface-on-surface separation.

### Dark Mode (Dark-to-Light Layering)

* **The Base:** The deepest layer (`surface-container-lowest` / `#0e0e0f`).
* **The Surface:** Floating panels step lighter to deep charcoal (`surface` / `#131314`) to "pop" forward.
* **The Highlight:** Interactive elements step lighter still (`surface-container-highest` / `#353436`).

### Light Mode (Light-to-Dark Layering)

* **The Base:** The clean, bright canvas surface (`surface-bright` / `#ffffff`).
* **The Surface:** Floating structures step down into subtle, concrete-toned silvers (`surface-container-low` / `#f3f3f4`) to drop away cleanly.
* **The Highlight:** Interactive wells and inputs sit in crisp inset containers (`surface-container-lowest` / `#ffffff`) or active hover blocks (`surface-container-highest` / `#e2e1e3`).
* **Glassmorphism:** The Floating Modality Dock (tool selector) uses a 35% alpha background blur. This allows the vector canvas to remain visible beneath the tools, reinforcing the "floating" nature of the UI.
* **No Borders:** Physical borders are set to 0px. Separation is achieved strictly through the delta between neighboring surface values.

---

## Components

### Buttons

* **Style:** Borderless, 8px rounded.
* **Resting:**
* *Dark:* `#1c1b1c` background with silver text.
* *Light:* `#ededee` background with dark charcoal text.


* **Hover:** Focused Teal container at 60% opacity (`primary-container` with alpha overlay).
* **Active:** Solid Focused Teal (`primary`) with highly contrasting text (`on-primary`).

### Numerical Inputs & Sliders

* **Style:** Horizontal layout, borderless frame.
* **Interactions:** Support "drag-to-scrub." The grab handle (slider thumb) uses a 6px rounding.
* **Color:** Handle uses the mode's designated `primary` teal, turning into `secondary` during active dragging.

### Floating Modality Dock

* **Style:** Pill-shaped horizontal bar, centered top.
* **Effect:** Background `surface` at 35% opacity with backdrop blur.
* **Active State:** The selected tool button takes a solid `primary` background with no gap to the dock edge, shifting text color to `on-primary`.

### Inspector Panels

* **Style:** 12px rounded windows when floating, 0px when docked.
* **Hierarchy:** Use uppercase `label-caps` for section titles (e.g., GEOMETRY, APPEARANCE) with 15px window padding. Background shifts to `surface-container-low` to anchor the layout relative to the canvas.

### Checkboxes & Radios

* **Style:** 8px rounded frames (no circles).
* **Indicator:** A solid `primary` checkmark or center square for selection.

### Layer Tree

* **Style:** Vertical list with no icons. Hovering a layer shows a soft `surface-container-highest` row highlight. Selected layers use a 70% opacity `primary-container` background to highlight the active branch instantly.