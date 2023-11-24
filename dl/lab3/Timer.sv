`include "lab3/Counter.sv"

//! 20MHz clk, time 1sec
module Timer #(
  CLK_FREQ = 20000000
) (
  input  logic clk,
  input  logic rst_n,
  input  logic en,
  output logic ready,
  output logic notify
);

logic [3:0] cnt_1;
logic [3:0] cnt_2;
logic [3:0] cnt_3;
logic [3:0] cnt_4;
logic [3:0] cnt_5;
logic [3:0] cnt_6;
logic [3:0] cnt_7;

logic r_ready;
logic r_notify;
logic trap;
logic rst;

assign rst = ~rst_n;
assign ready = r_ready;
assign notify = r_notify;

assign trap =
  cnt_1 == 4'((CLK_FREQ >>  0) & 15) &&
  cnt_2 == 4'((CLK_FREQ >>  4) & 15) &&
  cnt_3 == 4'((CLK_FREQ >>  8) & 15) &&
  cnt_4 == 4'((CLK_FREQ >> 12) & 15) &&
  cnt_5 == 4'((CLK_FREQ >> 16) & 15) &&
  cnt_6 == 4'((CLK_FREQ >> 20) & 15) &&
  cnt_7 == 4'((CLK_FREQ >> 24) & 15);

always @(posedge clk, posedge rst) begin
  if (rst) begin
    {r_ready, r_notify} <= 2'b10;
  end else if (en) begin
    {r_ready, r_notify} <= 2'b00;
  end else if (trap) begin
    {r_ready, r_notify} <= 2'b11;
  end
end

Counter u_cnt_1
(
  .clk   (clk && !r_ready),
  .rst_n (rst_n          ),
  .cnt   (cnt_1          )
);

Counter u_cnt_2
(
  .clk   (&~cnt_1),
  .rst_n (rst_n  ),
  .cnt   (cnt_2  )
);

Counter u_cnt_3
(
  .clk   (&~cnt_2),
  .rst_n (rst_n  ),
  .cnt   (cnt_3  )
);

Counter u_cnt_4
(
  .clk   (&~cnt_3),
  .rst_n (rst_n  ),
  .cnt   (cnt_4  )
);

Counter u_cnt_5
(
  .clk   (&~cnt_4),
  .rst_n (rst_n  ),
  .cnt   (cnt_5  )
);

Counter u_cnt_6
(
  .clk   (&~cnt_5),
  .rst_n (rst_n  ),
  .cnt   (cnt_6  )
);

Counter u_cnt_7
(
  .clk   (&~cnt_6),
  .rst_n (rst_n  ),
  .cnt   (cnt_7  )
);

endmodule : Timer
