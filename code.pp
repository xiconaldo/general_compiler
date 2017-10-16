program prog ;
var
    int0, int1, int2 :integer;
	real0, real1, real2 :real;
	int3 :integer;
  int4: integer;
  prog: integer;

procedure troca(valor_1, valor_2: integer);
  var aux, int0: integer;
      bool1: boolean;
      prog: integer;

  begin
    real0 := 5.0;
    int3 := 34;
    prog := int3;
    aux := valor_1;
    valor_1 := valor_2;
    {bool1 := aux * valor_1 + valor_2;}
    bool1 := false or false;
    bool1 := not false;

    if(bool1) then
      begin
        int3 := 4
      end
    else
      begin
        valor_1 := valor_2
      end
  end;

begin

  troca(int4, int4)

end.
