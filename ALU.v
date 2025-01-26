module ALU(  
    input [31:0] A,        // First operand
    input [31:0] B,        // Second operand
    input [1:0] opcode,    // Operation selector (2-bit control signal)
	 input sub,
	 input clk,
    output reg [31:0] result, // Result of the operation
    output reg carry_out       // Carry out for subtraction
    );
	 reg [31:0] out;
parameter ADDSUB = 2'b00;
parameter shift = 2'b01;
parameter AND = 2'b10;  // Shift left logical
parameter OR = 2'b11;

always@(posedge clk)begin

out <= result;

end

always @(*) begin
    carry_out = 0; // Default carry_out value
    case (opcode)
        ADDSUB: begin
		  if(sub)
            result = A - B; // Behavioral subtraction
            else result = A + B;
        end
        shift: begin
            result = {A[30:0],1'b0}; // Behavioral shift operation
        end
        AND: begin
            result = A & B; // Behavioral AND logical
        end
		  OR: begin
            result = A | B; // Behavioral OR logical
        end
        default: begin
            result = 32'b0; // Default case: output 0
        end
    endcase
end

endmodule