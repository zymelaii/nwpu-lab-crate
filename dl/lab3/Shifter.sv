module Shifter (
  input  logic      clk,
  input  logic      rst_n,
  input  logic[3:0] dat_in,
  input  logic      ser_in,
  output logic      ser_out
);

logic[3:0] data;
logic rst;

assign rst = ~rst_n;
assign ser_out = data[3];

always_ff @(posedge clk, posedge rst) begin
  if (rst) begin
    data <= dat_in;
  end else begin
    data <= {data[2:0], ser_in};
  end
end

endmodule : Shifter
