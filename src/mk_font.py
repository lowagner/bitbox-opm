#!/usr/bin/env python2

starting_index = 0

# font is 5x7, with a blank border.
# 5x6 = 30 bits
#   2 bits for the bottom row
#   0 = nothing on bottom row
#   1 = **___
#   2 = __***
#   3 = _***_

characters = [
   ["*   *", # 0 - null
    " *** ",
    "* * *",
    " * * ",
    " *** ",
    "*   *"],
   [" ** *", # null
    "*  **",
    "* **",
    " ** *",
    "**  *",
    "* **"],
   ["    ", # 2
    "    ",
    "    ",
    "    ",
    "*  *",
    " ** "],
   ["   *", # 3
    "  **",
    " *  ",
    "    ",
    "    ",
    "*   "],
   ["   *", # 4
    "  * ",
    "    ",
    "    ",
    "**  ",
    "*   "],
   [" *  ", # 5
    " *  ",
    "    ",
    "    ",
    " *  ",
    " ***"],
   ["****", # 6
    "*  *",
    "    ",
    "    ",
    "*  *",
    "*  *"],
   ["* * ", # 7
    " * *",
    "    ",
    "    ",
    "* * ",
    " * *"],
   ["    ", # 8
    "    ",
    "    ",
    "    ",
    "****",
    "****"],
   ["     ", # 9 \t tab
    "     ",
    "     ",
    "     ",
    "     ",
    "     ",
    "     "],
   ["     ", # 10 \n newline
    "     ",
    "     ",
    "     ",
    "     ",
    "     "],
   ["   *", 
    "   *",
    "    ",
    "    ",
    "  * ",
    "**  "],
   ["**  ", 
    "  * ",
    "   *",
    "    ",
    "    ",
    "   *"],
   ["", 
    "",
    "",
    "    ",
    "    ",
    ""],
   ["", 
    "",
    "    ",
    "    ",
    "",
    ""],
   ["", 
    "",
    "    ",
    "    ",
    "",
    ""],
   ["    ", # 16 
    " ** ",
    " ** ",
    "    ",
    "    ",
    "    "],
   ["    ", 
    "  **",
    "    ",
    "    ",
    "  **",
    "    "],
   ["    ", 
    "*  *",
    "*  *",
    "    "],
   ["    ", 
    "**  ",
    "**  ",
    "    "],
   ["    ", 
    "    ",
    " ** ",
    " ** "],
   ["    ", 
    "    ",
    "  **",
    "  **"],
   ["    ", 
    "    ",
    "*  *",
    "*  *"],
   ["    ", 
    "    ",
    "**  ",
    "**  "],
   [" ** ", 
    "    ",
    "    ",
    " ** "],
   ["  **", 
    "    ",
    "    ",
    "  **"],
   ["****", 
    "*  *",
    "*  *",
    "****"],
   ["**  ", 
    "    ",
    "    ",
    "**  "],
   [" ** ", 
    " ** ",
    "    ",
    "    "],
   ["  **", 
    "  **",
    "    ",
    "    "],
   ["*  *", 
    "*  *",
    "    ",
    "    "],
   ["**  ", 
    "**  ",
    "    ",
    "    "],
   ["     ", # ' ' (32)
    "     ",
    "     ",
    "     ",
    "     ",
    "     "],
   [" **  ", # !
    " **  ",
    " **  ",
    " **  ",
    "     ",
    " **  "],
   ["** **", # "
    "** **",
    " * * ",
    "     ",
    "     ",
    "     "],
   ["    ", # #
    " * *",
    "*****",
    " * *",
    "*****",
    " * * "],
   ["  *  ", # $
    " ****",
    " **  ",
    "  ** ",
    "**** ",
    "  *  "],
   ["**  *", # %
    "** **",
    "  **",
    " **  ",
    "** **",
    "*  **"],
   [" **  ", # &
    "** * ",
    " **  ",
    "** **",
    "** * ",
    " ** *"],
   [" *** ", # '
    " **  ",
    " *   ",
    "     ",
    "     ",
    "     "],
   ["   * ", # (
    " **  ",
    "***  ",
    "***  ",
    " **  ",
    "   * "],
   [" *   ", # )
    "  **  ",
    "  ***",
    "  ***",
    "  **  ",
    " *   "],
   ["     ",
    " * * ", # *
    "  *  ",
    " * * ",
    "     ",
    "     "],
   ["  *  ",
    "  *  ", # +
    "*****",
    "  *  ",
    "  *  ",
    "     "],
   ["     ", # ,
    "     ",
    "     ",
    "     ",
    " **  ",
    " **  ", "*"],
   ["     ", # -
    "     ",
    "*****",
    "     ",
    "     ",
    "     "],
   ["     ", # .
    "     ",
    "     ",
    "     ",
    " **  ",
    " **  "],
   ["    *", # /
    "   **",
    "  ** ",
    " **  ",
    "**   ",
    "*    "],
   [" *** ", # 0
    "**  *",
    "** **",
    "*** *",
    "**  *",
    " ***"],
   ["  ** ", # 1
    " *** ",
    "  ** ",
    "  ** ",
    "  ** ",
    "*****"],
   [" *** ", # 2
    "*  **",
    "  ** ",
    " **  ",
    "**   ",
    "*****"],
   ["**** ", # 3
    "   **",
    " *** ",
    "   **",
    "   **",
    "**** "],
   ["*  **", # 4
    "*  **",
    "*****",
    "   **",
    "   **",
    "   **"],
   ["*****", # 5
    "**   ",
    "**** ",
    "   **",
    "   **",
    "**** "],
   ["  ** ", # 6
    "**   ",
    "**** ",
    "**  *",
    "**  *",
    " ***"],
   ["*****", # 7
    "   **",
    "  ** ",
    " **  ",
    " **  ",
    " **  "],
   [" *** ", # 8
    "*  **",
    " *** ",
    "*  **",
    "*  **",
    " *** "],
   [" *** ", # 9
    "*  **",
    "*  **",
    " ****",
    "  ** " ,
    " **  "],
   ["     ", # :
    " **  ",
    " **  ",
    "     ",
    " **  ",
    " **  "],
   ["     ", # ;
    " **  ",
    " **  ",
    "     ",
    " **  ",
    " **  ", "*"],
   ["    *", # <
    "  ** ",
    "**   ",
    "  ** ",
    "    *",
    "     "],
   ["     ",
    "*****", # =
    "     ",
    "*****",
    "     ",
    "     "],
   ["*    ", # >
    " **  ",
    "   **",
    " **  ",
    "*    ",
    "     "],
   ["**** ", # ?
    "   **",
    "  ** ",
    " **  ",
    "     ",
    " ** "],
   [" *** ",
    "*   *", # @
    "*  **",
    "* * *",
    "* ***",
    "*    ", "**"],
   ["  *  ", # A
    "  ** ",
    " * **",
    "** **",
    "*****",
    "** **"],
   ["**** ", # B
    "**  *",
    "**** ",
    "**  *",
    "**  *",
    "****"],
   [" ****", # C
    "***  ",
    "***  ",
    "***  ",
    "***  ",
    " ****"],
   ["**** ", # D
    "**  *",
    "**  *",
    "**  *",
    "**  *",
    "**** "],
   ["*****", # E
    "**   ",
    "**** ",
    "**   ",
    "**   ",
    "*****"],
   ["*****", # F
    "**   ",
    "**   ",
    "**** ",
    "**   ",
    "**   "],
   [" *** ", # G
    "**  *",
    "**   ",
    "** **",
    "**  *",
    " *** "],
   ["*  **", # H
    "*  **",
    "*****",
    "*  **",
    "*  **",
    "*  **"],
   ["*****", # I
    "  ** ",
    "  ** ",
    "  ** ",
    "  ** ",
    "*****"],
   ["*****", # J
    "   **",
    "   **",
    "   **",
    "   **",
    "**** "],
   ["**  *", # K
    "** * ",
    "***  ",
    "***  ",
    "** * ",
    "**  *"],
   ["**   ", # L
    "**   ",
    "**   ",
    "**   ",
    "**   ",
    "*****"],
   ["*   *", # M
    "** **",
    "*****",
    "*****",
    "* * *",
    "*   *"],
   ["*   *", # N
    "**  *",
    "*** *",
    "* ***",
    "*  **",
    "*   *"],
   [" *** ", # O
    "**  *",
    "**  *",
    "**  *",
    "**  *",
    " *** "],
   ["**** ", # P
    "**  *",
    "**  *",
    "**** ",
    "**   ",
    "**   "],
   [" ***", # Q
    "*  **",
    "*  **",
    "*  **",
    "* ***",
    " *** ", " *"],
   ["**** ", # R
    "**  *",
    "**  *",
    "**** ",
    "**  *" ,
    "**  *"],
   [" ****", # S
    "**   ",
    "***  ",
    "  ***",
    "   **",
    "****"],
   ["*****", # T
    " **  ",
    " **  ",
    " **  ",
    " **  ",
    " **  "],
   ["*  **", # U
    "*  **",
    "*  **",
    "*  **",
    "*  **",
    " ****"],
   ["**  *", # V
    "**  *",
    "**  *",
    "**  *",
    " * * ",
    "  *  "],
   ["*   *", # W
    "* * *",
    "* * *",
    "* * *",
    "*****",
    "** **"],
   ["*   *", # X
    " * **",
    "  ** ",
    " **  ",
    "** * ",
    "*   *"],
   ["*  **", # Y
    "*  **",
    "*  **",
    " *** ",
    "  ** ",
    "  ** "],
   ["*****", # Z
    "   **",
    "  ** ",
    " **  ",
    "**   ",
    "*****"],
   ["**** ", # [
    "**   ",
    "**   ",
    "**   ",
    "**   ",
    "**** "],
   ["*    ", # \
    "**   ",
    " **  ",
    "  ** ",
    "   **",
    "    *"],
   [" ****", # ]
    "   **",
    "   **",
    "   **",
    "   **",
    " ****"],
   ["  *  ", # ^
    " * * ",
    "*   *",
    "     ",
    "     ",
    "     "],
   ["     ", # _
    "     ",
    "     ",
    "     ",
    "     ",
    "*****"],
   [" *** ", # `
    "  ** ",
    "   * ",
    "     ",
    "     ",
    "     "],
   ["     ", # a
    "     ",
    " ** *",
    "** **",
    "** **",
    " ** *"],
   ["**   ", # b
    "**   ",
    "**** ",
    "**  *",
    "**  *",
    "**** "],
   ["     ", # c
    "     ",
    "  ***",
    "**   ",
    "**   ",
    " ****"],
   ["   **", # d
    "   **",
    " ****",
    "*  **",
    "*  **",
    " ****"],
   ["     ", # e
    " *** ", 
    "**  *",
    "**** ",
    "**   ",
    "  ** "],
   ["  ***", # f
    " **  ",
    " **  ",
    "**** ",
    " **  ",
    " **  ", "* "],
   ["     ",
    "     ",
    " *** ", # g
    "*  **",
    " ****",
    "   **", "**"],
   ["**   ", # h
    "**   ",
    "**   ",
    "**** ",
    "**  *",
    "**  *"],
   [" **  ", # i
    "     ",
    "***  ",
    " **  ",
    " **  ",
    " ****"],
   ["  ** ", # j
    "     ",
    "  ** ",
    "  ** ",
    "  ** ",
    "  ** ", "**"],
   ["**   ", # k
    "**   ",
    "**  *",
    "**** ",
    "**** ",
    "**  *"],
   [" *** ", # l
    "  ** ",
    "  ** ",
    "  ** ",
    "  ** ",
    "  ***"],
   ["     ", # m
    "     ",
    "**** ",
    "* * *",
    "* * *",
    "* * *"],
   ["     ", # n
    "     ",
    "* ** ",
    "**  *",
    "**  *",
    "**  *"],
   ["     ", # o
    "     ",
    " *** ",
    "**  *",
    "**  *",
    " *** "],
   ["     ",
    "     ",
    "**** ", # p
    "**  *",
    "**** ",
    "**   ", "* "],
   ["     ", # q
    "     ",
    " ** *",
    "*  **",
    " ****",
    "   **", " *"],
   ["     ", # r
    "     ",
    "**** ",
    "**  *",
    "**   ",
    "**   "],
   ["     ",
    "     ",
    "  ***", # s
    " **  ",
    "  **",
    "***  "],
   ["     ", # t
    " **  ",
    "**** ",
    " **  ",
    " **  ",
    "  ** "],
   ["     ", # u
    "     ",
    "*  **",
    "*  **",
    "*  **",
    " ****"],
   ["     ", # v
    "     ",
    "**  *",
    "**  *",
    " * *",
    "  * "],
   ["     ",
    "     ",
    "*   *", # w
    "* * *",
    "* * *",
    "*****"],
   ["     ", # x
    "     ",
    "**  *",
    "  ** ",
    "  ** ",
    "**  *"],
   ["     ", # y
    "     ",
    " ** *",
    " ** *",
    " ** *",
    "  ** ", "**"],
   ["     ", # z
    "     ",
    "*****",
    "  ** ",
    " **  ",
    "*****"],
   ["   **", # {
    "  ** ",
    "  ** ",
    " **  ",
    "  ** ",
    "  ** ", " *"],
   ["  *  ", # |
    " *** ",
    " *** ",
    " *** ",
    " *** ",
    "  *  "],
   ["**   ", # }
    " **  ",
    " **  ",
    "  ** ",
    " ** ",
    " ** ", "*"],
   [" ** *", # ~
    "* ** ",
    "     ",
    "     "],
   ["    ", # 127 - down arrow
    "    ",
    " * *",
    "  * "],
   [" ** ", # 128 - integral
    " *  ",
    " *  ",
    "**  "],
   ["****", # 129 - Gamma (skip Alpha, Beta, they look like A, B)
    "**  ",
    "**  ",
    "**  "],
   ["  * ", # 130 - Delta
    " * *",
    "** *",
    "****"],
   ["****", # 131 - Theta (skip Epsilon, Zeta, Eta, they look like E, Z, H)
    "** *",
    "* **",
    "****"],
   [" *  ", # 132 - Lambda (skip Iota, Kappa, they look like l, K)
    "* * ",
    "* **",
    "* **"],
   ["****", # 133 - Xi (skip Mu, Nu, they look like M, N)
    " ** ",
    "    ",
    "****"],
   ["****", # 134 - Pi (skip Omicron, it looks like O)
    "* **",
    "* **",
    "* **"],
   ["****", # 135 - Sigma (skip Rho, it looks like P)
    " ** ",
    "*   ",
    "****"],
   ["* **", # 136 - Phi (skip Tau, Upsilon, they look like T, Y)
    " * *",
    "* * ",
    "** *"],
   ["*  *", # 137 - Psi (skip Chi, it looks like X)
    "* * ",
    " *  ",
    "* **"],
   [" ** ", # 138 - Omega
    "*  *",
    "** *",
    "** *"],
   ["", # 139
    "",
    "",
    ""],
   ["", # 140
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", # 144
    "",
    "",
    ""],
   ["   *", # 145 - alpha
    " ** ",
    "* * ",
    "** *"],
   [" ** ", # 146 - beta
    " ** ",
    " * *",
    "****"],
   ["*  *", # 147 - gamma
    " ** ",
    "* * ",
    "**  "],
   [" *  ", # 148 - delta
    " ** ",
    "* **",
    " ** "],
   [" ** ", # 149 - epsilon
    " ***",
    "*   ",
    " ** "],
   [" ***", # 150 - zeta
    "*   ",
    "*** ",
    "   * "],
   ["*** ", # 151 - eta
    "*  *",
    "*  *",
    "   * "],
   [" ** ", # 152 - theta
    "** *",
    "* **",
    " ** "],
   ["    ", # 153 - iota
    " ** ",
    " ** ",
    " ** "],
   ["    ", # 154 - kappa
    "** *",
    "*** ",
    "** *"],
   ["**  ", # 155 - lambda
    "  * ",
    " * *",
    "*  *"],
   ["    ", # 156 - mu
    "*  *",
    "****",
    "   *"],
   ["    ", # 157 - nu
    "** *",
    " ** ",
    " *  "],
   ["**  ", # 158 - xi
    "*** ",
    "*   ",
    " ***"],
   ["    ", # 159 - skip omicron, go to pi
    "****",
    "* * ",
    "* * "],
   ["  * ", # 160 - rho 
    " * *",
    " ** ",
    "**  "],
   ["    ", # 161 - sigma (circle)
    " ***",
    "* * ",
    " *  "],
   [" ** ", # 162 - sigma (open)
    "*   ",
    " ***",
    "   *"],
   ["    ", # 163 - tau
    "****",
    " ** ",
    " ** "],
   ["    ", # 164 - upsilon
    "*  *",
    " * *",
    " ** "],
   ["*   ", # 165 - phi
    " ** ",
    " ** ",
    "   *"],
   ["** *", # 166 - chi
    " ** ",
    "* * ",
    "  **"],
   [" * *", # 167 - psi
    " ** ",
    " ***",
    "*   "],
   ["    ", # 168 - omega
    "*  *",
    "** *",
    " ** "],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["", 
    "",
    "",
    ""],
   ["****", # 224 - RANDOMIZE 0
    "****",
    "****",
    "****"],
   ["* * ",  # RANDOMIZE 1
    " * *",
    "* * ",
    " * *"],
   [" * *", # RANDOMIZE 2
    "* * ",
    " * *",
    "* * "],
   ["   *", # RANDOMIZE 3
    "*   ",
    "    ",
    " *  "],
   ["    ", # RANDOMIZE 4
    "    ",
    "****",
    "****"],
   ["    ", # RANDOMIZE 5
    "****",
    "****",
    "    "],
   ["****", # RANDOMIZE 6
    "****",
    "    ",
    "    "],
   ["****", # RANDOMIZE 7
    "    ",
    "    ",
    "****"],
   ["    ", # RANDOMIZE 8
    "    ",
    "    ",
    "****"],
   ["    ", # RANDOMIZE 9
    "    ",
    "****",
    "    "],
   ["    ", # RANDOMIZE 10
    "****",
    "    ",
    "    "],
   ["****", # RANDOMIZE 11
    "    ",
    "    ",
    "    "],
   ["*   ", # RANDOMIZE 12
    "*   ",
    "*   ",
    "*   "],
   [" *  ", # RANDOMIZE 13
    " *  ",
    " *  ",
    " *  "],
   ["  * ", # RANDOMIZE 14
    "  * ",
    "  * ",
    "  * "],
   ["   *", # RANDOMIZE 15
    "   *",
    "   *",
    "   *"], 
   ["    ", # 240 -> zero
    "    ",
    "    ",
    "    "],
   ["    ", # 241
    "    ",
    "    ",
    "****"],
   ["    ", # 242
    "    ",
    "****",
    "    "],
   ["    ", # 243
    "    ",
    "****",
    "****"],
   ["    ", # 244
    "****",
    "    ",
    "    "],
   ["    ", # 245
    "****",
    "    ",
    "****"],
   ["    ", # 246
    "****",
    "****",
    "    "],
   ["    ", # 247
    "****",
    "****",
    "****"],
   ["****", # 248
    "    ",
    "    ",
    "    "],
   ["****", # 249
    "    ",
    "    ",
    "****"],
   ["****", # 250
    "    ",
    "****",
    "    "],
   ["****", # 251
    "    ",
    "****",
    "****"],
   ["****", # 252
    "****",
    "    ",
    "    "],
   ["****", # 253
    "****",
    "    ",
    "****"],
   ["****", # 254 
    "****",
    "****",
    "    "],
   ["*****", # 255
    "*****",
    "*****",
    "*****",
    "*****",
    "*****"],
]

with open("src/font.c", 'w') as f:
    f.write('// autogenerated by mk_font.py')
    f.write("#include \"font.h\"\n#include \"common.h\"\n#include \"../config.h\"\n\n")
    f.write("uint32_t font[256] CCM_MEMORY;\n")
    f.write("uint32_t font_cache[256] = {\n")
    if starting_index:
        f.write("[%d]="%starting_index)
    else:
        f.write("  ")
    for i in range(len(characters)):
        char = characters[i]
        x = 0
        for j in range(len(char)):
            power = 5*j
            for c in char[j]:
                if c != ' ':
                    x |= 1<<power
                power += 1
        if i + 1 == len(characters):
            f.write("%du\n"%x)
        else:
            f.write("%du,\n  "%x)
    f.write("};\n")
    if len(characters) + starting_index > 256:
        print "WARNING, overflow!"
    f.write("void font_init()\n{\n    memcpy(font, font_cache, sizeof(font_cache));\n}\n")
    f.write("""

uint32_t font_pixel_doubles[4] CCM_MEMORY;

void font_set_line_color(uint16_t fg, uint16_t bg)
{
    font_pixel_doubles[0] = bg | (bg<<16);
    font_pixel_doubles[1] = fg | (bg<<16);
    font_pixel_doubles[2] = bg | (fg<<16);
    font_pixel_doubles[3] = fg | (fg<<16);
}

void font_render_line(const char *text, int text_len, int y)
{
    // y should never be < 0.  TEXT_OFFSET_X should be even.
    uint32_t *dst = (uint32_t *)(&draw_buffer[TEXT_OFFSET_X]);
    if (y >= 7)
    for (; text_len>0; --text_len)
    {
        *dst++ = font_pixel_doubles[0];
        *dst++ = font_pixel_doubles[0];
        *dst++ = font_pixel_doubles[0];
    }
    else if (y == 6)
    for (; text_len>0; --text_len)
    switch (font[(uint8_t)(*text++)]>>((FONT_WIDTH-1)*(FONT_HEIGHT-1)))
    {
        case 0:
            *dst++ = font_pixel_doubles[0];
            *dst++ = font_pixel_doubles[0];
            *dst++ = font_pixel_doubles[0];
            break;
        case 1:
            *dst++ = font_pixel_doubles[3];
            *dst++ = font_pixel_doubles[0];
            *dst++ = font_pixel_doubles[0];
            break; 
        case 2:
            *dst++ = font_pixel_doubles[0];
            *dst++ = font_pixel_doubles[2];
            *dst++ = font_pixel_doubles[1];
            break; 
        case 3:
            *dst++ = font_pixel_doubles[3];
            *dst++ = font_pixel_doubles[1];
            *dst++ = font_pixel_doubles[0];
            break; 
    }
    else
    for (; text_len>0; --text_len)
    {
        uint32_t character = font[(uint8_t)(*text++)] >> ((FONT_WIDTH-1)*y);
        *dst++ = font_pixel_doubles[character&3];
        *dst++ = font_pixel_doubles[(character>>2)&3];
        *dst++ = font_pixel_doubles[(character>>4)&1];
    }
}
""")

with open("src/font.h", 'w') as f:
    f.write('// autogenerated by mk_font.py')
    f.write("#pragma once\n#include <stdint.h>\n")
    f.write("extern uint32_t font[256];\nvoid font_init();\nextern uint32_t font_pixel_doubles[4];\n")
    f.write("void font_render_line(const char *text, int text_len, int y);\n")
    f.write("void font_set_line_color(uint16_t fg, uint16_t bg);\n")
