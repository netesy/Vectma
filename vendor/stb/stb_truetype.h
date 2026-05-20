#ifndef STB_TRUETYPE_H
#define STB_TRUETYPE_H

// Mocked version for the sandbox environment
#include <vector>
#include <string>

typedef struct {
   unsigned char *data;
} stbtt_fontinfo;

inline int stbtt_InitFont(stbtt_fontinfo *info, const unsigned char *data, int offset) { (void)info; (void)data; (void)offset; return 1; }
inline int stbtt_GetGlyphShape(const stbtt_fontinfo *info, int glyph_index, void **vertices) { (void)info; (void)glyph_index; (void)vertices; return 0; }
inline void stbtt_FreeShape(const stbtt_fontinfo *info, void *vertices) { (void)info; (void)vertices; }

#endif
