`timescale 1ns/1ps
module top();
   reg         clk;
   reg         resetn;
   reg         load_en;
   reg   [7:0] data_in;
   wire [15:0] load_counter_out;
  
  initial begin
    clk=0;
    resetn=0;
    load_en = 0;
    
    #20 resetn = 1'b1;
  end
  
  always #10 clk = ~clk;
  always #50 load_en = load_en;
  
  reg[31:0] a;
  reg [3:0] c = 2;
  reg[3:0] d = 10;
  initial begin
    wait(resetn)
    begin
      // User defined function for writing data
      // Defined in C-language
      
      a = $pow_x(c,d);
      $display($pow_x(c,d));
//       $display("Function output is :: %0d", a);
      $finish();
    end
      
    
  end
endmodule // top
