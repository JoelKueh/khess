import gdb
import gdb.printing


class BitboardPrinter:
    def __init__(self, value):
        self.value = value

    def to_string(self):
        stream = gdb.parse_and_eval("gdb_get_output_stream()")
        gdb.set_convenience_variable("gdb_stream", stream)
        gdb.set_convenience_variable("gdb_value", self.value)
        gdb.parse_and_eval(f"cb_print_bitboard($gdb_stream, $gdb_value)")
        buffer = gdb.parse_and_eval("gdb_get_output_buffer()")
        return f"{int(self.value)}\n{buffer.string()}"


class BoardPrinter:
    def __init__(self, value):
        self.value = value

    def to_string(self):
        stream = gdb.parse_and_eval("gdb_get_output_stream()")
        gdb.set_convenience_variable("gdb_stream", stream)
        gdb.set_convenience_variable("gdb_value", self.value)

        gdb.parse_and_eval("cb_print_board_ascii($gdb_stream, $gdb_value)")
        board = gdb.parse_and_eval("gdb_get_output_buffer()")

        gdb.parse_and_eval("cb_print_state($gdb_stream, $gdb_value)")
        state = gdb.parse_and_eval("gdb_get_output_buffer()")

        hist_top = gdb.parse_and_eval("($gdb_value).hist.data[($gdb_value).hist.count-1]")
        gdb.set_convenience_variable("gdb_hist_top", self.value)
        gdb.parse_and_eval("cb_print_history($gdb_stream, $gdb_hist_top)")

        return f"{int(self.value)}\n{buffer.string()}"


class HistoryPrinter:
    """Pretty-printer for cb_history_t."""
    def __init__(self, value):
        self.value = value

    def to_string(self):
        stream = gdb.parse_and_eval("gdb_get_output_stream()")
        gdb.set_convenience_variable("gdb_stream", stream)
        gdb.set_convenience_variable("gdb_value", self.value)

        hist_top = gdb.parse_and_eval("($gdb_value).hist.data[($gdb_value).hist.count-1]")
        gdb.set_convenience_variable("gdb_hist_top", self.value)
        gdb.parse_and_eval("cb_print_history($gdb_stream, $gdb_hist_top)")

        return f"{int(self.value)}\n{buffer.string()}"



class MovePrinter:
    def __init__(self, value):
        self.value = value

    def to_string(self):
        buffer = gdb.parse_and_eval("(char[6]){0}")
        result = gdb.parse_and_eval(f"cb_mv_to_uci_algbr({self.value}, {buffer.address})")
        return f"{int(self.value)}: {result.string()}"


def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("chess")

    pp.add_printer(
        "bitboard pretty printer",
        r"^bitboard_t$",
        BitboardPrinter,
    )

    pp.add_printer(
        "chess board pretty printer",
        r"^cb_board_t$",
        BoardPrinter,
    )

    pp.add_printer(
        "chess history pretty printer",
        r"^cb_history_t$",
        HistoryPrinter,
    )

    pp.add_printer(
        "move pretty printer",
        r"^cb_move_t$",
        MovePrinter,
    )

    return pp


gdb.printing.register_pretty_printer(
    None,
    build_pretty_printer(),
)
