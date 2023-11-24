module Counter (
  input  logic      clk,
  input  logic      rst_n,
  output logic[3:0] cnt
);

logic[3:0] data;
logic rst;

assign rst = ~rst_n;
assign cnt = data;

always_ff @(posedge clk, posedge rst) begin
  if (rst) begin
    data <= 4'b0;
  end else begin
    data <= data + 1;
  end
end

endmodule : Counter
