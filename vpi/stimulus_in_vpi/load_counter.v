module load_counter(
input wire clk,
input wire resetn,
input wire load_en,
input wire [9:0] data_in,
output reg [9:0] count_out);

always @(posedge clk, negedge resetn)
begin
   if(~resetn)
      count_out <= 0;
   else
   begin
      count_out <= count_out + 1'b1;
      if(load_en)
         count_out <= data_in;
   end
end
endmodule //load_counter
