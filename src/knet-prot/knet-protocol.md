
# Knet Protocol

Responsible for translating 
Protocol for passing messages from the khess server to a khess client.

## Implementation

All communication is done via TCP sockets with a pipe/newline-delimited, message stream.

### Client Messages

#### move

Full format: `move|<UCI_ALGBR>|<FULLMV_NUM>`

Declares the intent of the client to make a particular move. This is usually sent in response to
a `go` command from the server but can also be sent preemptively to handle pre-moves. Server
should respond with an ack message.

Arguments:

* FULLMV_NUM
* UCI_ALGBR: 

#### undo

Full format: `undo`

Requires no arguments, pops off the top element of the pre-move stack. Server should respond with
an ack message. Does nothing if there is no move to undo.

#### update

Full format: `update`

Requires no arguments, sent by the client to request an update on the game state. The server will
respond with a `pgn` command containing the full PGN string that represents the game.

#### resign

Full format: `resign`

Requires no arguments, sent by the client to indicate a resignation.

#### offer-draw

Full format: `offer-draw`

Requires no arguments, sent by the client to offer a draw.

#### accept-draw

Full format: `accept-draw`

Requires no arguments, sent by the client to accept a draw offer.

#### request-draw

Full format: `reject-draw`

Requires no arguments, sent by the client to reject a draw offer.

#### join

Full format: `join|<ROOM_ID>`

Attempt to join a room. 

#### create

Full format: `create|`

### Server Responses

#### ack-move

Full format: `ack|<FULLMV_NUM>|<ACK_STATUS>`

Acknowledges a move.

Arguments:
* HALFMV_NUM: The halfmv number of the move that the server is replying to as a base 10 integer.
* ACK_STATUS: An enum representing the status of the command passed as a base 10 integer.
    * OK - The move was applied successfully.
    * POSTPONE - The decision about the move was postponed (e.g. put on the pre-move stack).
    * INVALID - The move was invalid and another must be supplied.

#### ack-craete

Full format: `ack|<STATUS_CODE>`

Sent as a response to any create-room

#### ack-join

Full format: `ack|<STATUS_CODE>`

Sent as a response to any join command.

#### start-uci-fen

Full format: `start-uci-fen`

A message to the client that the steam is entering UCI FEN mode. All subsequent messages should now
be considered part of a UCI FEN string. FEN mode ends upon the transmission of three newlines in a
row: `... FEN MODE ... \n\n\n ... NORMAL MODE ...`.


