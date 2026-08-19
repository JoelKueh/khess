import gdb
import gdb.printing


class BitboardPrinter:
    """Pretty-printer for a uint64_t interpreted as a chess bitboard."""

    FILES = "abcdefgh"

    def __init__(self, value):
        self.value = value

    def to_string(self):
        value = int(self.value)

        rows = []
        for rank in range(7, -1, -1):
            squares = []
            for file in range(8):
                square = rank * 8 + file
                squares.append("1" if value & (1 << square) else ".")

            rows.append(f"{rank + 1} " + " ".join(squares))

        rows.append("  " + " ".join(self.FILES))
        return "\n".join(rows)


class HistoryPrinter:
    """Pretty-printer for cb_history_t.

    Bit layout:
        HLFMV_NUMBER : ENP_COL / CAP_PIECE : ENP_AVAIL : KQkq
    """

    PIECES = {
        0: "P",  # CB_PTYPE_PAWN
        1: "N",  # CB_PTYPE_KNIGHT
        2: "B",  # CB_PTYPE_BISHOP
        3: "R",  # CB_PTYPE_ROOK
        4: "Q",  # CB_PTYPE_QUEEN
        5: "K",  # CB_PTYPE_KING
        6: "-",  # CB_PTYPE_EMPTY
    }

    def __init__(self, value):
        self.value = value

    def to_string(self):
        num = int(self.value)

        castling = ""
        if num & (1 << 0):
            castling += "K"
        if num & (1 << 1):
            castling += "Q"
        if num & (1 << 2):
            castling += "k"
        if num & (1 << 3):
            castling += "q"

        if not castling:
            castling = "-"

        enp_avail = num & (1 << 4)
        tagged_value = num & (0b111 << 5)

        if enp_avail:
            enp_col = tagged_value
            enp_or_cap = chr(ord("a") + enp_col)
        else:
            cap_piece = tagged_value
            enp_or_cap = self.PIECES.get(cap_piece, "?")

        return f"{num} : {enp_or_cap} : {enp_avail} : {castling}"


def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("chess")

    pp.add_printer(
        "bitboard pretty printer",
        r"^bitboard_t$",
        BitboardPrinter,
    )

    pp.add_printer(
        "chess history pretty printer",
        r"^cb_history_t$",
        HistoryPrinter,
    )

    return pp


def enable_chess_bitboards():
    gdb.printing.register_pretty_printer(
        gdb.current_objfile(),
        build_pretty_printer(),
    )
