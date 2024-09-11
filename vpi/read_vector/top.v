module top;

reg [10:0] vector;
reg clk;

initial begin
   clk = 0;
   forever #10 clk = ~clk;
end

always @(posedge clk)  $read_vector("file1.txt", vector);
initial begin
   $monitor("FROM SIM :: VECTOR = 0x%0h", vector);
end

endmodule // top
