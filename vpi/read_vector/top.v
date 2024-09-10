module top;

reg [10:0] vector;
reg clk;

initial begin
   clk = 0;
   forever #10 clk = ~clk;
end

initial begin
   repeat(10)
   @(posedge clk) read_vector_vpi("file1.txt", vector);
   #100 $finish();
end

initial begin
   $monitor("FROM SIM :: VECTOR = 0x%0h", vector);
end

endmodule // top
