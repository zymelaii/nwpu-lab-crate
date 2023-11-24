`include "lab3/Shifter.sv"
`include "lab3/Counter.sv"

//! first high level to activate the serdes
module SerDes (
  input  logic      clk,
  input  logic      rst_n,
  input  logic      ser_in,
  output logic      en_rd,
  output logic[3:0] par_out
);

logic rst;
logic ready;
logic[3:0] cnt;
logic[3:0] data;

assign rst = ~rst_n;
assign en_rd = ready;
assign par_out = data;

Shifter u_shifter
(
  .clk     (clk   ),
  .rst_n   (rst_n ),
  .dat_in  (4'd0  ),
  .ser_in  (ser_in),
  .ser_out (      )
);

Counter u_cnt
(
  .clk   (clk  ),
  .rst_n (rst_n),
  .cnt   (cnt  )
);

always_ff @(posedge rst, posedge cnt[0]) begin
  if (rst) begin
    ready <= 0;
    data <= 0;
  end else if (cnt == 5 && !ready) begin
    ready <= 1;
    data <= u_shifter.data;
  end
end

endmodule : SerDes
