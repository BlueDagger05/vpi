module top;

initial begin
   $file_op("file1.txt", "file2.txt");

   #100 $finish();
end

endmodule // top
