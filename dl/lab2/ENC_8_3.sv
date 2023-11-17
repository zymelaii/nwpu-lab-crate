module ENC_8_3 (
  input  logic[7:0] dat_in,
  output logic[2:0] dat_out,
  output logic      err
);

always_comb begin
  unique casez (dat_in)
    8'b00000001: {err, dat_out} = 4'd0;
    8'b00000010: {err, dat_out} = 4'd1;
    8'b00000100: {err, dat_out} = 4'd2;
    8'b00001000: {err, dat_out} = 4'd3;
    8'b00010000: {err, dat_out} = 4'd4;
    8'b00100000: {err, dat_out} = 4'd5;
    8'b01000000: {err, dat_out} = 4'd6;
    8'b10000000: {err, dat_out} = 4'd7;
    default: {err, dat_out} = 4'd8;
  endcase
end

endmodule : ENC_8_3
