//! dat_in(0), dat_in(1) == 00 | 11 -> dat_out = 1
//! otherwise -> dat_out = 0
module CustomFSM (
  input  logic clk,
  input  logic rst,
  input  logic dat_in,
  output logic dat_out
);

typedef enum logic[2:0] { S_INIT, S_00, S_01, S_10, S_11 } state_t;

state_t state;
state_t next_state;

always_ff @(posedge clk, posedge rst) begin
  if (rst) begin
    state <= S_INIT;
  end else begin
    state <= next_state;
  end
end

always_comb begin
  unique casez (state)
    S_INIT:  next_state = dat_in == 0 ? S_00 : S_01;
    S_00:    next_state = dat_in == 0 ? S_00 : S_01;
    S_01:    next_state = dat_in == 0 ? S_10 : S_11;
    S_10:    next_state = dat_in == 0 ? S_00 : S_01;
    S_11:    next_state = dat_in == 0 ? S_10 : S_11;
    default: next_state = S_INIT;
  endcase
  unique casez (state)
    S_INIT:  dat_out = 0;
    S_00:    dat_out = 1;
    S_01:    dat_out = 0;
    S_10:    dat_out = 0;
    S_11:    dat_out = 1;
    default: dat_out = 0;
  endcase
end

endmodule : CustomFSM
