create schema regtest;
set search_path = regtest;

/************* SQL/PSM ... PLPSM0 regress tests **********
 * these tests shows only correctly writen code.
 **********************************************************
 */
create or replace function test01(a int)
returns int as $$
begin
  while a > 0 do
    set a = a - 2;
  end while;
  return a;
end;
$$ language psm0;

create or replace function test02(a int)
returns int as $$
begin
  while $1 > 0 do
    set $1 = $1 - 2;
  end while;
  return $1;
end;
$$ language psm0;

create or replace function test03(a int)
returns int as $$
begin
  loop
    if a = -5 then
      return a;
    end if;
    set a = a - 1;
  end loop;
end;
$$ language psm0;

create or replace function test04(a int)
returns int as $$
begin
  declare i, s int default 0;
  repeat
    set s = s + i;
    set i = i + 1;
  until i >= a end repeat;
  return s;
end;
$$ language psm0;

create or replace function test05(a int)
returns int as $$
return a;
$$ language psm0;

create or replace function test06(a text)
returns text as $$
return 'Hello ' || a;
$$ language psm0;

create or replace function test06_01(out a text) as $$
set a = 'Hello World';
$$ language psm0;

create or replace function test06_02(inout a text) as $$
set a = 'Hello ' || a;
$$ language psm0;

create or replace function test06_03(out a text, out b text) as $$
set a = 'Hello', b = 'World';
$$ language psm0;

create or replace function test06_04(out a text, out b text) as $$
set (a, b) = ('Hello', 'World');
$$ language psm0;

create or replace function test06_05(a int)
returns int as $$
begin
  declare x,y,z int;
  set (x,y,z) = (0, 1, 2 + a);
  set y = y + z, x = x + y;
  return x;
end;
$$ language psm0;

-- recursion call test
create or replace function test06_06(a int)
returns int as $$
if a > 1 then
  return a * test06_06(a - 1);
else
  return 1;
end if;
$$ language psm0;

create or replace function test06_07(a int)
returns int as $$
begin
  declare x,y,z int;
  set (x,y,z) = (select 10,20,a+10);
  return x + y + z;
end;
$$ language psm0;

create or replace function test07(a int)
returns int as $$
  x1: begin
        declare a int default 7;
    x2: begin
          declare a int default 5;
          return a;
        end;
      end;
$$ language psm0;

create or replace function test08(a int)
returns int as $$
  x1: begin
        declare a int default 7;
    x2: begin
          declare a int default 5;
          return $1;
        end;
      end;
$$ language psm0;

create or replace function test09(a int)
returns int as $$
  x1: begin
        declare a int default 7;
    x2: begin
          declare a int default 5;
          return test09.a + x1.a + x2.a;
        end;
      end;
$$ language psm0;

create or replace function test10(a int)
returns int as $$
  x1: begin
        declare a int default 7;
    x2: begin
          declare a int default 5;
          return test10.a + x1.a + a;
        end;
      end;
$$ language psm0;

create or replace function test11(a int)
returns int as $$
    begin
      declare b int default 3;
  x1: begin
        declare b int default 7;
        print b;
      end;
  x2: begin
        return a + b;
      end;
    end;
$$ language psm0;

create or replace function test12(a int)
returns int as $$
begin
  declare b,c,d int default 0;
  set b = a + 1, c = b + 1, d = c + 1;
  return d;
end;
$$ language psm0;

create or replace function test13(inout a int) as $$
set a = a + 10;
$$ language psm0;

create or replace function test14(out a int, out b int) as $$
set a = 1, b = 2;
$$ language psm0;

create or replace function test15(a int)
returns int as $$
begin
x1:while $1 > 0 do
     set $1 = $1 - 1;
     if $1 < 10 then
       leave x1;
     end if;
   end while;
   return $1;
end;
$$ language psm0;

create or replace function test16(inout a int)
returns int as $$
x1: begin
      set a = a + 1;
      leave x1;
      set a = a + 1;
    end;
$$ language psm0;

create or replace function test17(a int)
returns int as $$
  begin
    declare i int default a + 10;
x1: loop
      set i = i - 1;
      if i > a then
        iterate x1;
      end if;
      if i < 1 then
        leave x1;
      end if;
    end loop;
    return i;
  end;
$$ language psm0;

create or replace function test18(inout a int) as $$
x1:while a > 0 do
     if a = 3 then
       leave x1;
     end if;
     set a = a - 1;
   end while;
$$ language psm0;

create or replace function test19(out a int, out b int) as $$
  begin
    set a = 0, b = 0;
x1: loop
  x2: loop
        if a = 5 then
          leave x2;
        end if;
        set b = b + 1;
        if b = 5 then
          set a = a + 1, b = 0;
          iterate x2;
        end if;
      end loop x2;
      set a = a + 10;
      leave x1;
    end loop x1;
  end;
$$ language psm0;

create or replace function test20(out a int, out b int) as $$
  begin
    set a = 0, b = 0;
x1: loop
  x2: loop
        if a = 5 then
          leave x1;
        end if;
        set b = b + 1;
        if b = 5 then
          set a = a + 1, b = 0;
          iterate x2;
        end if;
      end loop x2;
      set a = a + 10;
      leave x1;
    end loop x1;
  end;
$$ language psm0;

create or replace function test21(a int)
returns int as $$
case a
      when 1 then return 11;
      when 2 then return 12;
      when 3 then return 13;
             else return 14;
end case;
$$ language psm0;

create or replace function test22(inout a int)
returns int as $$
case a
      when 1,2,3 then set a = a + 10;
                 else set a = a - 10;
end case;
$$ language psm0;

create or replace function test23(a int)
returns int as $$
case
      when a = 1 then set a = a + 10; return a;
      when a = 2 then set a = a + 10; return a;
      when a = 3 then set a = a + 10; return a;
             else return -1;
end case;
$$ language psm0;

create table footab(a int);
insert into footab values(1),(2),(3),(4);

create or replace function test24(a int)
returns int as $$
begin
  declare a int;
  set a = (select footab.a from footab where footab.a = test24.a);
  return a;
end;
$$ language psm0;

create or replace function test25(a int)
returns int as $$
  return (select footab.a from footab where footab.a = test25.a);
$$ language psm0;

create type xx as (a integer, b integer);

create or replace function test26()
returns int as $$
begin
  declare a xx;
  declare s int default 0;
  if a is null then set s = s + 1; end if;
  set a = (10,20)::xx;
  if a.a + a.b = 30 then set s = s + 1; end if;
  if a.a = 40 then set s = -1; end if;
  set a = null;
  if a is null then set s = s + 1; end if;
  return s;
end;
$$ language psm0;

create or replace function test26_01(a int, b int)
returns int as $$
begin
  declare x xx;
  set x = (a,b);
  set (x.a, x.b) = (x.a + 1, x.b + 1);
  set x.a = x.a + 1;
  set x.b = x.b + 1;
  return x.a + x.b * 10;
end;
$$ language psm0;

create or replace function test27(a int)
returns int as $$
  begin
    declare x int;
    declare s int default 0;
    declare cx cursor for select i from generate_series(1,10) g(i) where i > a;
    open cx;
l1: loop
      fetch cx into x;
      /*
       * Atention: this pattern isn't possible on MySQL, where fetching no data
       * raises a error. But it isn't compatibility problem, because every MySQL's
       * procedure will use a continue or exit handler. Correct procedure will not
       * stop on unhandled warning. Because we like to be near DB2, then we doesn't
       * follow MySQL behave and we doesn't raise break here.
       */
      if x is null then
        leave l1;
      end if;
      set s = s + x;
    end loop;
    return s;
  end;
$$ language psm0;

create table footab2(a int);
insert into footab2 values(1),(2),(3);

create or replace function test28(a int)
returns int as $$
begin
  declare sqlstate char(5);
  declare aux int;
  declare s int default 0;
  declare cx cursor for select footab.a from footab where footab.a <> test28.a;
  open cx;
  fetch cx into aux;
  while sqlstate = '00000' do
    set s = s + aux;
    fetch cx into aux;
  end while;
  close cx;
  return s;
end;
$$ language psm0;

-- out sqlcode will be zero, because 'not found' sqlcode
-- is replaced by success from test "sqlcode = 0"
create or replace function test30(out x int, out s int, out sqlcode int, out sqlstate char(5))
as $$
begin
  declare cx cursor for select a from footab;
  open cx;
  set s = 0;
  fetch cx into x;
  while sqlcode = 0 do
    print s;
    set s = s + x;
    fetch cx into x;
  end while;
end;
$$ language psm0;

create or replace function test31()
returns int as $$
begin
  declare sqlstate char(5);
  declare i int default 0;
  declare a1, a2 int;
  declare c1 cursor for select a from footab;
  declare c2 cursor for select a from footab2;
  open c1;
  fetch c1 into a1;
  while sqlstate = '00000' do
    open c2;
    fetch c2 into a2;
    while sqlstate = '00000' do
      set i = i + 1;
      fetch c2 into a2;
    end while;
    fetch c1 into a1;
  end while;
  return i;
end;
$$ language psm0;

create or replace function test31_01()
returns int as $$
begin
  declare s int default  0;
  declare x xx;
  declare sqlcode int;
  declare c2 cursor for select a, a + 1 from footab2;
  open c2;
  fetch c2 into x.a, x.b;
  while sqlcode = 0 do
    set s = s + x.a + x.b;
    fetch c2 into x.a, x.b;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test32()
returns int as $$
begin
  declare sqlcode int;
  declare i int default 0;
  declare a1, a2 int;
  declare c1 cursor for select a from footab;
  declare c2 cursor for select a from footab2;
  open c1;
  fetch c1 into a1;
  while sqlcode = 0 do
    open c2;
    fetch c2 into a2;
    while sqlcode = 0 do
      set i = i + 1;
      fetch c2 into a2;
    end while;
    fetch c1 into a1;
  end while;
  return i;
end;
$$ language psm0;

create or replace function test32_01()
returns int as $$
begin
  declare sqlcode int;
  declare s int default 0;
  declare a1, a2 int;
  declare c1 cursor for select a from footab order by a;
  declare c2 cursor for select a from footab2 order by a;
  open c1;
  fetch c1 into a1;
  while sqlcode = 0 do
    open c2;
    fetch c2 into a2;
    while sqlcode = 0 do
      set s = s + a1 * a2;
      fetch c2 into a2;
    end while;
    fetch c1 into a1;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test33()
returns int as $$
begin
  execute immediate 'create table foo(a int)';
  execute immediate 'insert into foo values(10)';
  execute immediate 'drop table foo';
  return 0;
end;
$$ language psm0;

create or replace function test34()
returns int as $$
begin
  prepare xx from 'select 10 + $1';
  prepare xx from 'select 20 + $1';
  return 0;
end;
$$ language psm0;

/*
 * Change to DB2:
 * only reference to variable can used in prepare statement in DB2,
 */
create or replace function test35(a float, b float)
returns int as $$
begin
  declare x int;
  prepare xx from 'select 10 + $1';
  execute xx into x using b;
  return x;
end;
$$ language psm0;

create type footype as (a int, b int);

create or replace function test35(a int, b footype)
returns int as $$
begin
  declare x int;
  prepare xx from 'select 10 + $1';
  execute xx into x using b.b;
  return x;
end;
$$ language psm0;

create or replace function test35_01(a int, b int)
returns footype as $$
begin
  declare x footype;
  select (a, b) into x;
  return x;
end;
$$ language psm0;

create or replace function test36(t text)
returns int as $$
begin
  declare s int default 0;
  declare par int default 3;
  declare sqlcode int;
  declare aux int;
  declare c cursor for xx;
  prepare xx from 'select * from ' || quote_ident(t) || ' where a = $1';
  open c using par;
  fetch c into aux;
  while sqlcode = 0 do
    set s = s + aux;
    fetch c into aux;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test36_01(t text)
returns int as $$
begin
  declare s int default 0;
  declare par int default 3;
  declare sqlcode int;
  declare aux int;
  declare c cursor for xx;
  prepare xx from 'select * from ' || quote_ident(t) || ' where a <> $1';
  open c using par;
  fetch c into aux;
  while sqlcode = 0 do
    set s = s + aux;
    fetch c into aux;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test37()
returns int as $$
begin
  declare s int default 0;
  for ft as c1 cursor for select * from footab do
    set s = s + ft.a;
  end for;
  return s;
end;
$$ language psm0;

/*
 * NOT_FOUND event of FOR stmt isn't possible handled by custom
 * condition handlers!
 */
create or replace function test37_00()
returns int as $$
begin
  declare s int default 0;
  declare continue handler for not found return -1;
  for ft as c1 cursor for select * from footab do
    set s = s + ft.a;
  end for;
  return s;
end;
$$ language psm0;

create or replace function test37_01()
returns int as $$
begin
  declare s int default 0;
  for ft as c1 cursor for select * from footab do
    set s = s + a;
  end for;
  return s;
end;
$$ language psm0;

create or replace function test37_02()
returns int as $$
begin
  declare s int default 0;
  for select * from footab do
    set s = s + a;
  end for;
  return s;
end;
$$ language psm0;

create or replace function test37_03()
returns int as $$
begin
   declare s int default 0;
x1:for select * from footab do
     set s = s + a;
     if s > 5 then
       leave x1;
     end if;
   end for;
   return s;
end;
$$ language psm0;

create or replace function test37_04()
returns int as $$
begin
   declare s int default 0;
x1:for select * from footab do
     set s = s + a;
     if s > 5 then
       leave x1;
     end if;
   end for x1;
   return s;
end;
$$ language psm0;

create or replace function test37_05()
returns int as $$
begin
   declare s int default 0;
x1:for select * from footab do
     set s = s + a;
     iterate x1; -- actually must not change a result
   end for x1;
   return s;
end;
$$ language psm0;

create or replace function test37_06()
returns int as $$
begin
    declare s int;
    set s = 0;
x1: begin
  x2: for select * from footab do
        set s = s + a;
        if s > 5 then
          leave x1;
        end if;
      end for x2;
      set s = s + 10;
    end;
    return s;
end;
$$ language psm0;

create or replace function test37_07()
returns int as $$
begin
    declare s int;
    set s = 0;
x1: begin
  x2: for select * from footab  where a = 100 do
        set s = s + a;
        if s > 5 then
          leave x1;
        end if;
      end for x2;
      set s = s + 10;
    end;
    return s;
end;
$$ language psm0;

create or replace function test38()
returns int as $$
begin
  declare aux int;
  declare s int default 0;
  declare done boolean default false;
  declare c1 cursor for select a from footab;
  declare continue handler for not found set done = true;
  open c1;
  fetch c1 into aux;
  while not done do
    set s = s + aux;
    fetch c1 into aux;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test38_01()
returns int as $$
 begin
   declare aux int;
   declare s int default 0;
   declare done boolean;
   declare c cursor for select * from footab;
   declare continue handler for not found set done = true;
   open c;
x1:loop
     fetch c into aux;
     if done then
       close c;
       leave x1;
     end if;
     set s = s + aux;
   end loop;
   return s;
 end;
$$ language psm0;

create or replace function test38_02()
returns int as $$
begin
  declare aux int;
  declare s int default 0;
  declare sqlstate char(5);
  declare sqlcode int;
  declare done boolean default false;
  declare c1 cursor for select a from footab;
  declare continue handler for not found set done = true;
  open c1;
  fetch c1 into aux;
  while not done and sqlstate = '00000' and sqlcode = 0 do
    set s = s + aux;
    fetch c1 into aux;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test38_03(out s int)
as $$
begin
  declare aux int;
  declare c cursor for select a from footab;
  declare continue handler for not found return;
  set s = 0;
  open c;
  loop
    fetch c into aux;
    set s = s + aux;
  end loop;
end;
$$ language psm0;

create or replace function test39()
returns int as $$
x1:begin
     declare a int;
     declare s int default 0;
     declare done boolean default false;
     declare c1 cursor for select f.a from footab f order by 1;
     declare continue handler for not found set done = true;
     open c1;
     fetch c1 into a;
     while not done do
       begin
         declare a int;
         declare done boolean default false;
         declare c2 cursor for select f.a from footab2 f order by 1;
         declare continue handler for not found set done = true;
         open c2;
         fetch c2 into a;
         while not done do
           set s = s + x1.a * a;
           fetch c2 into a;
         end while;
       end;
       fetch c1 into a;
     end while;
     return s;
   end;
$$ language psm0;

create or replace function test40()
returns int as $$
begin
  declare a1, a2 int;
  declare s int default 0;
  declare done boolean default false;
  declare c1 cursor for select f.a from footab f order by 1;
  declare c2 cursor for select f.a from footab2 f order by 1;
  declare continue handler for not found set done = true;
  open c1;
  fetch c1 into a1;
  while not done do
    open c2;
    fetch c2 into a2;
    while not done do
      set s = s + a1 * a2;
      fetch c2 into a2;
    end while;
    close c2;
    set done = false;
    fetch c1 into a1;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test41()
returns int as $$
begin
  declare aux int;
  declare s int default 0;
  declare done boolean default false;
  declare c1 cursor for select a from footab;
  declare continue handler
     for not found
     begin
       while s < 100 do
         set s = s + 1;
       end while;
       set done = true;
     end;
  open c1;
  fetch c1 into aux;
  while not done do
    set s = s + aux;
    fetch c1 into aux;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test42()
returns int as $$
begin
  declare aux int;
  declare s int default 0;
  declare done boolean default false;
  declare c1 cursor for select a from footab;
  declare continue handler
     for not found
     begin
     x1:while s < 100 do
          set s = s + 1;
            if s > 90 then
              leave x1;
            end if;
        end while x1;
        set done = true;
     end;
  open c1;
  fetch c1 into aux;
  while not done do
    set s = s + aux;
    fetch c1 into aux;
  end while;
  return s;
end;
$$ language psm0;

create or replace function test43()
returns int as $$
begin
  declare aux int;
  declare s int default 0;
  declare done boolean default false;
  declare c1 cursor for select a from footab;
  declare continue handler
     for not found
     begin
     x1:while s < 100 do
          set s = s + 1;
            if s > 90 then
              leave x1;
            end if;
        end while x1;
        set done = true;
     end;
  open c1;
  fetch c1 into aux;
  while not done do
    set s = s + aux;
    fetch c1 into aux;
  end while;
  set s = s + 10;
  return s;
end;
$$ language psm0;

create or replace function test44()
returns int as $$
begin
  declare aux int;
  declare s int default 0;
  declare done boolean default false;
  declare c1 cursor for select a from footab;
  declare continue handler for not found return s;
  open c1;
  fetch c1 into aux;
  while not done do
    set s = s + aux;
    fetch c1 into aux;
  end while;
  set s = s + 33;
  return s;
end;
$$ language psm0;

create or replace function test50(p int)
returns int as $$
begin
  declare s int default 0;
  delete from footab;
  insert into footab values(p),(p);
  update footab set a = p + 10;
  for select a from footab do
    set s = s + a;
  end for;
  return s;
end;
$$ language psm0;

create or replace function test51()
returns int as $$
begin
  declare result int;
  select a into result from footab order by 1 limit 1;
  return result;
end;
$$ language psm0;

create or replace function test52()
returns int as $$
begin
  declare result int;
  select 33 into result;
  return result;
end;
$$ language psm0;

create or replace function test53()
returns int as $$
begin
  declare result numeric;
  select a into result from footab order by 1 limit 1;
  return result;
end;
$$ language psm0;

create or replace function test53_01()
returns int as $$
begin
  declare result numeric;
  declare _result int;
  select a,a into result,_result from footab order by 1 limit 1;
  return result + _result;
end;
$$ language psm0;

create or replace function test54()
returns int as $$
begin
  declare sqlcode int;
  declare result numeric;
  delete from footab;
  print sqlcode;
  select a into result from footab order by 1 limit 1;
  print sqlcode;
  if sqlcode <> 0 then
    return -1;
  else
    return +1;
  end if;
end;
$$ language psm0;

-- using a subselect in assign statement newer raise a handler
create or replace function test55()
returns int as $$
begin
  declare ret int default 100;
  declare continue handler for not found set ret = -100;
  delete from footab;
  set ret = (select a from footab limit 1); -- subselect just returns NULL;
  return coalesce(ret, -1);
end;
$$ language psm0; --returns -1

-- using a select into raise a handler
create or replace function test56()
returns int as $$
begin
  declare ret int default 100;
  declare continue handler for not found set ret = -100;
  delete from footab;
  select a into ret from footab; -- raise error, when query returns more than one row
  return coalesce(ret, -1);
end;
$$ language psm0; -- returns -100

create or replace function test56_01()
returns int as $$
begin
  declare ret int;
  declare continue handler for not found return -100;
  delete from footab;
  select a into ret from footab; -- raise error, when query returns more than one row
  return coalesce(ret, -1);
end;
$$ language psm0; -- returns -100

create or replace function test57()
returns int as $$
begin
  declare ret int default 100;
  select a into ret from footab;
  return coalesce(ret, -1);
end;
$$ language psm0; -- returns -1

create or replace function test58()
returns int as $$
begin
  declare x xx;
  select 10,20 into x.a, x.b;
  return x.a + x.b;
end;
$$ language psm0;

create or replace function test58_01(out x xx)
returns xx as $$
  select 10,20 into x.a, x.b;
$$ language psm0;

create or replace function test59(in n int)
returns int as $$
begin
  declare i, p, d int;
  declare s bigint default 0;
  declare found boolean;
  set i = 0, p = 2;
  while i < n do
nt: loop
      set p = p + 1, d = p / 2;
      while d > 1 do
        if p % d = 0 then
          iterate nt;
        end if;
        set d = d - 1;
      end while;
      leave nt;
    end loop;
    -- print i, p;
    set (s, i) = (s + p, i + 1);
  end while;
  return s;
end;
$$ language psm0;

-- check a exit handler
create or replace function test60(out s int)
as $$
begin
  declare aux int;
  declare c1 cursor for select a from footab;
  declare exit handler for not found begin end;
  set s = 0;
  open c1;
  fetch c1 into aux;
  loop
    set s = s + aux;
    fetch c1 into aux;
  end loop;
end;
$$ language psm0;

create or replace function test60_1(out s int)
as $$
begin
  declare aux int;
  declare c1 cursor for select a from footab;
  declare exit handler for sqlwarning begin end;
  set s = 0;
  open c1;
  fetch c1 into aux;
  loop
    set s = s + aux;
    fetch c1 into aux;
  end loop;
end;
$$ language psm0;

create or replace function test60_2(out s int)
as $$
begin
  declare aux int;
  declare c1 cursor for select a from footab;
  declare exit handler for sqlwarning begin end;
  begin
    set s = 0;
    open c1;
    fetch c1 into aux;
    loop
      set s = s + aux;
      fetch c1 into aux; -- we leave a block where exit handler is defined, not this
    end loop;
  end;
  set s = -10; -- must not executed
end;
$$ language psm0;

create or replace function test60_3(out s int)
as $$
begin
  declare aux int;
  declare c1 cursor for select a from footab;
  declare exit handler for sqlwarning set s = s * 100;
  declare exit handler for not found set s = s * 10; -- prefered handler
  begin
    set s = 0;
    open c1;
    fetch c1 into aux;
    loop
      set s = s + aux;
      fetch c1 into aux; -- we leave a block where exit handler is defined, not this
    end loop;
  end;
end;
$$ language psm0;

create or replace function test60_4(out s int)
as $$
begin
  declare aux int;
  declare c1 cursor for select a from footab;
  declare exit handler for not found set s = s * 10; -- prefered handler
  declare exit handler for sqlwarning set s = s * 100; -- order isn't important
  begin
    set s = 0;
    open c1;
    fetch c1 into aux;
    loop
      set s = s + aux;
      fetch c1 into aux; -- we leave a block where exit handler is defined, not this
    end loop;
  end;
end;
$$ language psm0;

create or replace function test61(a int, out s integer)
returns int as $$
xx:begin
     set s = 0;
     begin atomic
       declare c cursor for select * from footab;
       open c;
       print 'Hello';
       set s = 10;
       if a > 10 then leave xx; end if;
       print 'World';
       set s = s + 10;
     end;
     print 'Hello World again';
     set s = s + 10;
   end xx;
$$ language psm0;

create or replace function test61_a(a int, out s integer)
returns int as $$
xx:begin
     set s = 0;
     begin atomic
       declare x int;
       declare c cursor for select * from footab;
       declare exit handler for not found begin print 'Ahoj'; end;
       open c;
       print 'Hello';
       set s = 10;
       fetch c into x;
       if a > 10 then leave xx; end if;
       print 'World';
       set s = s + 10;
     end;
     print 'Hello World again';
     set s = s + 10;
   end xx;
$$ language psm0;

create table tab62_c(a int check (a > 10));
create table tab62_n(a int);

create or replace function test62(a int, out result integer)
returns int as $$
begin atomic
  declare undo handler for sqlexception
     begin
       declare continue handler for not found
         print 'not found';
       print 'sql exception';
     end;
  begin atomic
    declare undo handler for sqlstate '55555'
      print 'sqlstate 55555';
    declare continue handler for not found
      print 'not found';
    print 'some statement from nested block';
  end;
  print 'some statement from outer block';
end;
$$ language psm0;

create or replace function test63(a int, out result integer) as $$
begin atomic
  declare undo handler for sqlexception
    set result = -1;
  set result = 10 / a;
  set result = result + 2;
end;
$$ language psm0;

create or replace function test63_1(a int, out result integer) as $$
begin atomic
  declare undo handler for sqlstate '22012'
    set result = -1;
  declare undo handler for sqlexception
    set result = -2;
  set result = 10 / a;
  set result = result + 2;
end;
$$ language psm0;

create or replace function test63_2(a int, out result integer) as $$
begin atomic
  declare undo handler for sqlexception
    set result = -2;
  declare undo handler for sqlstate '22012'
    set result = -1;
  set result = 100000 / a;
  set result = result + 2;
end;
$$ language psm0;

create or replace function test63_3(a int, out result integer) as $$
begin atomic
  declare undo handler for sqlexception
    set result = -2;
  declare undo handler for sqlstate '22012'
    set result = -1;
  set result = 100000 * a;
  set result = result + 2;
end;
$$ language psm0;

create or replace function test63_4(a int, out result integer) as $$
begin atomic
  declare undo handler for sqlstate '22012'
    set result = -1;
  declare undo handler for sqlexception
    set result = -2;
  set result = 100000 * a;
  set result = result + 2;
end;
$$ language psm0;

create or replace function test64()
returns int as $$
begin
  signal sqlstate '00001';
  return 1;
end;
$$ language psm0;

create or replace function test64_1()
returns int as $$
begin
  signal sqlstate '02001';
  return 1;
end;
$$ language psm0;

create or replace function test64_2(out a int) as $$
begin
  declare continue handler for sqlstate '02001'
    set a = a + 1;
  set a = 10;
  signal sqlstate '02001';
  set a = a + 1;
end;
$$ language psm0;

create or replace function test64_3(out a int) as $$
begin
  declare exit handler for sqlstate '02001'
    set a = a + 1;
  set a = 10;
  signal sqlstate '02001';
  set a = a + 1;
end;
$$ language psm0;

create or replace function test64_33(out a int) as $$
begin
  declare exit handler for sqlstate '02001'
    set a = a + 1;
  set a = 10;
  signal sqlstate '02002';
  signal sqlstate '02001';
  set a = a + 1;
end;
$$ language psm0;

create or replace function test64_4(out a int) as $$
begin
  declare exit handler for sqlwarning
    set a = a * 10;
  declare exit handler for sqlstate '02001'
    set a = a + 1;
  set a = 10;
  signal sqlstate '02002';
  signal sqlstate '02001';
  set a = a + 1;
end;
$$ language psm0;

create or replace function test64_5(par int, out a int) as $$
begin atomic
  declare undo handler for sqlwarning
    set a = a * 10;
  declare exit handler for sqlstate '02001'
    set a = a + 3;
  set a = 10;
  if par = 1 then
    signal sqlstate '02002';
  elseif par = 2 then
    signal sqlstate '02001';
  end if;
  set a = a + 1;
end;
$$ language psm0;

create or replace function test65(a int, out r int) as $$
begin
  set r = 0;
x: while a > 0 do
     begin atomic
       if a = 5 then
         leave x;
       end if;
       set a = a - 1;
       set r = r + 1;
     end;
   end while;
   set r = r + 1;
end;
$$ language psm0;

create or replace function test65_1(a int, out r int) as $$
begin
  declare counter int default 10;
  declare exit handler for sqlstate '02001'
    set r = r + 100;
  set r = 0;
x: while counter > 0 do
     begin atomic
       if counter = a then
         signal sqlstate '02001';
       end if;
       set counter = counter - 1;
       set r = r + 1;
     end;
   end while;
   set r = r + 1;
end;
$$ language psm0;

create or replace function test65_2(a int, out r int) as $$
begin atomic
  declare counter int default 10;
  declare undo handler for sqlstate '03001'
    set r = r + 100;
  set r = 0;
x: while counter > 0 do
     begin atomic
       if counter = a then
         signal sqlstate '03001';
       end if;
       set counter = counter - 1;
       set r = r + 1;
     end;
   end while;
   set r = r + 1;
end;
$$ language psm0;

create or replace function test65_3(a int, out r int) as $$
begin atomic
  declare counter int default 10;
  declare exit handler for sqlstate '02001'
  begin
    print 'catch warning';
    set r = r + 100;
    set r = r + 100;
  end;
  set r = 0;
x: while counter > 0 do
     begin atomic
       if counter = a then
         signal sqlstate '02001';
       end if;
       set counter = counter - 1;
       set r = r + 1;
     end;
   end while;
   set r = r + 1;
end;
$$ language psm0;

create or replace function test66(a int, out r int) as $$
begin
  declare continue handler for sqlstate '01002'
    set r = r + 1;
  declare continue handler for sqlstate '01003'
    set r = r + 2;
  set r = 0;
x: while a > 0 do
     if a % 2 = 0 then
       signal sqlstate '01002';
     else
       signal sqlstate '01003';
     end if;
     set a = a - 1;
   end while;
end;
$$ language psm0;

create or replace function test67(a int, out r int) as $$
begin
  declare exit handler for sqlwarning
    set r = r + 5;
  begin
    declare exit handler for sqlstate '02002'
       begin
         set r = r + 1;
         signal sqlstate '02002';
       end;
    set r = 0;
    set r = r + 1;
    if a = 1 then
      signal sqlstate '02002';
    elseif a = 2 then
      signal sqlstate '02003';
    end if;
  end;
  set r = r + 100;
end;
$$ language psm0;

create or replace function test67_1(a int, out r int) as $$
begin atomic
  declare undo handler for sqlexception
    set r = r + 5;
  begin atomic
    declare undo handler for sqlstate '03002'
       begin
         set r = r + 1;
         signal sqlstate '03002';
       end;
    set r = 0;
    set r = r + 1;
    if a = 1 then
      signal sqlstate '03002';
    elseif a = 2 then
      signal sqlstate '03003';
    end if;
  end;
  set r = r + 100;
end;
$$ language psm0;

-- handler cannot be called recursive
create or replace function test68(out r int) as $$
  begin atomic
    declare undo handler for sqlexception
      begin
        print 'handler 0';
        set r = r + 100;
      end;
    begin atomic
      declare undo handler for sqlstate '03002'
      begin atomic
        declare undo handler for sqlstate '03002'
          begin
            print 'nested handler';
            -- resignal 
            set r = r + 1;
            signal sqlstate '03003';
          end;
        print 'handler 1';
        set r = r + 1;
        signal sqlstate '03002';
      end;
      print 'nested block';
      set r = 0;
      signal sqlstate '03002';
    end;
  end;
$$ language psm0;

-- handlers doesn't process a signals from same
-- compound statement handler's body. It protect us
-- against to cross recursive calls.
create or replace function test68_1(out r int) as $$
  begin
    declare continue handler for sqlstate '01001'
      begin
        print 'handler 0';
        set r = r + 100;
      end;
    begin atomic
      declare continue handler for sqlstate '01001'
        begin
          -- must not be called
          print 'same compound statement handler';
          set r = r + 50;
        end;
      declare undo handler for sqlstate '03002'
      begin atomic
        declare undo handler for sqlstate '03002'
          begin
            print 'nested handler';
            -- resignal 
            set r = r + 1;
            /*
             * This signal isn't handled by some nested handler
             * so we have to search out of current compound
             * statement - and there is "handler 0"
             */
            signal sqlstate '01001';
            set r = r + 1;
          end;
        print 'handler 1';
        set r = r + 1;
        signal sqlstate '03002';
      end;
      print 'nested block';
      set r = 0;
      signal sqlstate '03002';
    end;
  end;
$$ language psm0;

create or replace function test69()
returns text as $$
begin
  declare sqlstate char(5);
  declare sqlcode int;
  declare continue handler for sqlwarning
  begin
    print sqlcode, sqlstate;
    return sqlstate;
  end;
  print 'start';
  signal sqlstate '02002';
end;
$$ language psm0;

create or replace function test69_1()
returns text as $$
begin atomic
  declare sqlstate char(5);
  declare sqlcode int;
  declare undo handler for sqlexception
  begin
    print sqlcode, sqlstate;
    return sqlstate;
  end;
  print 'start';
  signal sqlstate '03002';
end;
$$ language psm0;

--
-- print doesn't modify a sqlstate or sqlcode
--
create or replace function test69_2(a int)
returns text as $$
begin atomic
  declare sqlstate char(5);
  declare sqlcode int;
  declare aux int;
  declare undo handler for sqlexception
  begin
    print sqlcode, sqlstate;
    return sqlstate;
  end;
  print 'start';
  set aux = 10 / a;
  return '00000';
end;
$$ language psm0;

create or replace function test70(out r text) as $$
begin
  declare exit handler for sqlwarning
      get diagnostics r = returned_sqlstate;
  signal sqlstate '02002';
end;
$$ language psm0;

create or replace function test70_1(out r int) as $$
begin
  declare sqlstate char(5);
  declare sqlcode int;
  declare exit handler for sqlwarning
    begin
      print sqlstate, sqlcode;
      get diagnostics r = returned_sqlcode;
    end;
  signal sqlstate '02002';
end;
$$ language psm0;

--
-- get diagnostics statement doesn't modify first_area
--
create or replace function test70_2(out r int) as $$
begin
  declare sqlstate char(5);
  declare sqlcode int;
  declare exit handler for sqlwarning
    begin
      get diagnostics r = returned_sqlcode;
      print sqlstate, sqlcode;
    end;
  signal sqlstate '02002';
end;
$$ language psm0;

create or replace function test71(out s int, out _sqlcode int, out _sqlstate text)
as $$
begin
  declare aux int;
  declare c1 cursor for select a from footab;
  declare exit handler for not found
    begin
      get diagnostics _sqlcode = returned_sqlcode, _sqlstate = returned_sqlstate;
    end;
  set s = 0;
  open c1;
  fetch c1 into aux;
  loop
    set s = s + aux;
    fetch c1 into aux;
  end loop;
end;
$$ language psm0;

create or replace function test71_1(out s int, out _sqlcode int, out _sqlstate text, out _message text)
as $$
begin atomic
  declare aux int;
  declare sqlcode int;
  declare c1 cursor for select a from footab;
  declare undo handler for sqlstate '55001'
    begin
      get diagnostics _sqlcode = returned_sqlcode, _sqlstate = returned_sqlstate, _message = message_text;
    end;
  set s = 0;
  open c1;
  loop
    fetch c1 into aux;
    if sqlcode <> 0 then
      signal sqlstate '55001' set message_text = 'HANDLED NOT FOUND, ALL IS OK';
    end if;
    set s = s + aux;
  end loop;
end;
$$ language psm0;

create or replace function test71_2(out s int, out _sqlcode int, out _sqlstate text, out _message text)
as $$
begin atomic
  declare aux int;
  declare sqlcode int;
  declare sqlstate char(5);
  declare msg text default 'My dynamic message';
  declare c1 cursor for select a from footab;
  declare undo handler for sqlstate '55001'
    begin
      get diagnostics _sqlcode = returned_sqlcode, _sqlstate = returned_sqlstate, _message = message_text;
    end;
  set s = 0;
  open c1;
  loop
    fetch c1 into aux;
    get diagnostics _sqlcode = returned_sqlcode, _sqlstate = returned_sqlstate;
    if _sqlcode <> 0 then
      set msg = msg || ' ' || _sqlstate;
      signal sqlstate '55001' set message_text = msg;
    end if;
    set s = s + aux;
  end loop;
end;
$$ language psm0;

create or replace function test71_3(a int)
returns text as $$
begin atomic
  declare _sqlstate text;
  declare aux int;
  declare undo handler for sqlexception
  begin
    get diagnostics _sqlstate = returned_sqlstate;
    return _sqlstate;
  end;
  set aux = 10 / a;
  return '00000';
end;
$$ language psm0;

create or replace function test72()
returns text as $$
begin atomic
  declare r text;
  declare s text;
  declare undo handler for sqlexception
    begin
      get diagnostics r = returned_sqlstate;
      set s = s || ',' || r;
      return s;
    end;
  begin atomic
    declare undo handler for sqlexception
      begin
        get diagnostics r = returned_sqlstate;
        set s =  r;
        signal sqlstate '33333';
        resignal;
      end;
    signal sqlstate '66550';
  end;
end;
$$ language psm0;

create or replace function test72_1()
returns text as $$
begin atomic
  declare r text;
  declare s text;
  declare undo handler for sqlexception
    begin
      get diagnostics r = returned_sqlstate;
      set s = s || ',' || r;
      return s;
    end;
  begin atomic
    declare undo handler for sqlexception
      begin
        get diagnostics r = returned_sqlstate;
        set s =  r;
        resignal;
      end;
    signal sqlstate '66550';
  end;
end;
$$ language psm0;

create or replace function test72_2()
returns text as $$
begin atomic
  declare r text;
  declare s text;
  declare undo handler for sqlexception
    begin
      get diagnostics r = returned_sqlstate;
      set s = s || ',' || r;
      return s;
    end;
  begin atomic
    declare undo handler for sqlexception
      begin
        get diagnostics r = returned_sqlstate;
        set s =  r;
        resignal sqlstate '44444';
      end;
    signal sqlstate '66550';
  end;
end;
$$ language psm0;

-- signaling via condition and HT
create or replace function test72_3()
returns text as $$
begin atomic
  declare r text;
  declare s text;
  declare custom_condition condition;
  declare undo handler for sqlexception
    begin
      get diagnostics r = returned_sqlstate;
      set s = s || ',' || r;
      return s;
    end;
  begin atomic
    declare undo handler for sqlexception
      begin
        get diagnostics r = returned_sqlstate;
        set s =  r;
        resignal;
      end;
    signal custom_condition;
  end;
end;
$$ language psm0;

create or replace function test72_4()
returns text as $$
begin atomic
  declare r text;
  declare s text;
  declare custom_condition condition for sqlstate '56444';
  declare undo handler for sqlexception
    begin
      get diagnostics r = returned_sqlstate;
      set s = s || ',' || r;
      return s;
    end;
  begin atomic
    declare undo handler for sqlexception
      begin
        get diagnostics r = returned_sqlstate;
        set s =  r;
        resignal;
      end;
    signal custom_condition;
  end;
end;
$$ language psm0;

create or replace function test73()
returns text as $$
begin
  declare result text default '';
  declare continue handler for sqlwarning
    begin
      declare aux text;
      get stacked diagnostics aux = returned_sqlstate;
      set result = result || ' ' || aux;
      get current diagnostics aux = returned_sqlstate;
      set result = result || ' ' || aux;
      get stacked diagnostics aux = returned_sqlstate;
      set result = result || ' ' || aux;
    end;
  signal sqlstate '02005';
  signal sqlstate '02005';
  return result;
end;
$$ language psm0;

create or replace function test73_1()
returns text as $$
begin atomic
  declare result text default '';
  declare undo handler for sqlexception
    begin
      declare aux text;
      get stacked diagnostics aux = returned_sqlstate;
      set result = result || ' ' || aux;
      get current diagnostics aux = returned_sqlstate;
      set result = result || ' ' || aux;
      get stacked diagnostics aux = returned_sqlstate;
      set result = result || ' ' || aux;
      return result;
    end;
  signal sqlstate '45005';
  return result; -- dead code
end;
$$ language psm0;

create or replace function test74()
returns text as $$
begin
  declare not_found condition for sqlstate '02000';
  declare exit handler for not_found
    begin
      print 'not found handler activated';
      return 'Signal handled';
    end;
  signal sqlstate '02000';
end;
$$ language psm0;

create or replace function test74_1()
returns text as $$
begin
  declare not_found condition for sqlstate '02000';
  declare exit handler for not_found
    begin
      declare xx text;
      get diagnostics xx = condition_identifier;
      return xx || ' Signal handled';
    end;
  signal not_found;
end;
$$ language psm0;

create or replace function test74_2()
returns text as $$
begin atomic
  declare not_found condition for sqlstate '03000';
  declare undo handler for not_found
    begin
      declare xx, yy text;
      get stacked diagnostics xx = condition_identifier, yy = returned_sqlstate;
      return xx || ' Signal handled ' || yy;
    end;
  signal not_found;
end;
$$ language psm0;

-- check buildin conditions
create or replace function test75(in a int)
returns int as $$
begin atomic
  declare undo handler for division_by_zero
    return null;
  return 10 / a;
end $$ language psm0;


/*
 * example of before trigger

create or replace function trg01()
returns trigger as $$
begin
  declare n footab as new;
  declare o footab as old;
  if n.i <> o.i then
    -- skip this record
    signal sqlstate '02099';
  end if;
  set n.j = o.j;
end;
$$ language psm0;

*/

/*************************************************
 * Assert functions - sure, it is in plgsql :)
 */
create or replace function assert(text, int, int)
returns void as $$
begin
  if ($2 <> $3) then
    raise exception 'test "%" broken. %<>%', $1, $2, $3;
  end if;
end;
$$ language plpgsql;

create or replace function assert(text, bool, bool)
returns void as $$
begin
  if ($2 <> $3) then
    raise exception 'test "%" broken. %<>%', $1, $2, $3;
  end if;
end;
$$ language plpgsql;


create or replace function assert(text, text, text)
returns void as $$
begin
  if ($2 <> $3) then
    raise exception 'test "%" broken "%" <> "%"', $1, $2, $3;
  end if;
end;
$$ language plpgsql;

select * from footab;
select * from footab2;

create or replace function test()
returns void as $$
begin
  perform assert('test01', -1, test01(11));
  perform assert('test02', -1, test02(11));
  perform assert('test03', -5, test03(11));
  perform assert('test04', 45, test04(10));
  perform assert('test05', 11, test05(11));
  perform assert('test06', 'Hello Hello', test06('Hello'));
  perform assert('test06_01', 'Hello World', test06_01());
  perform assert('test06_02', 'Hello World', test06_02('World'));
  perform assert('test06_03', 'Hello', (test06_03()).a);
  perform assert('test06_04', 'Hello', (test06_04()).a);
  perform assert('test06_05', 8, test06_05(5));
  perform assert('test06_06', 720, test06_06(6));
  perform assert('test06_07', 50, test06_07(10));
  perform assert('test07',  5, test07(3));
  perform assert('test08',  3, test08(3));
  perform assert('test09', 15, test09(3));
  perform assert('test10', 15, test10(3));
  perform assert('test11', 13, test11(10));
  perform assert('test12',  4, test12(1));
  perform assert('test13', 10, test13(0));
  perform assert('test14',  2, (test14()).b);
  perform assert('test15',  7, test15(8));
  perform assert('test15',  9, test15(12));
  perform assert('test16',  4, test16(3));
  perform assert('test17',  0, test17(2));
  perform assert('test17', -5, test17(-5));
  perform assert('test18',  0, test18(2));
  perform assert('test18',  3, test18(10));
  perform assert('test19', 15, (select a from test19()));
  perform assert('test20',  5, (select a from test20()));
  perform assert('test21', 11, test21(1));
  perform assert('test21', 12, test21(2));
  perform assert('test21', 13, test21(3));
  perform assert('test21', 14, test21(4));
  perform assert('test21', 14, test21(5));
  perform assert('test22', 11, test22(1));
  perform assert('test22', 12, test22(2));
  perform assert('test22', 13, test22(3));
  perform assert('test22', -6, test22(4));
  perform assert('test22', -5, test22(5));
  perform assert('test23', 11, test23(1));
  perform assert('test23', 12, test23(2));
  perform assert('test23', 13, test23(3));
  perform assert('test23', -1, test23(4));
  perform assert('test23', -1, test23(5));
  perform assert('test24',  2, test24(2));
  perform assert('test24', -1, coalesce(test24(10), -1));
  perform assert('test25',  2, test25(2));
  perform assert('test25', -1, coalesce(test25(10),-1));
  perform assert('test26', 3, test26());
  perform assert('test26_01', 232, test26_01(10,20));
  perform assert('test27', 40, test27(5));
  perform assert('test28',  6, test28(4));
  perform assert('test30', 10, (select s from test30()));
  perform assert('test30', 0,(select sqlcode from test30()));
  perform assert('test30', '00000', (select sqlstate from test30()));
  perform assert('test31', 12, test31());
  perform assert('test31_01', 15, test31_01());
  perform assert('test32', 12, test32());
  perform assert('test32_01', 60, test32_01());
  perform assert('test33',  0, test33());
  perform assert('test34',  0, test34());
  perform assert('test35', 30, test35(10,20));
  perform assert('test35', 40, test35(0, (20,30)));
  perform assert('test35_01', 10, (test35_01(10,20)).a);
  perform assert('test36',  3, test36('footab'));
  perform assert('test36_01', 7, test36_01('footab'));
  perform assert('test37', 10, test37());
  perform assert('test37_00', 10, test37_00());
  perform assert('test37_01', 10, test37_01());
  perform assert('test37_02', 10, test37_02());
  perform assert('test37_03',  6, test37_03());
  perform assert('test37_04',  6, test37_04());
  perform assert('test37_05', 10, test37_05());
  perform assert('test37_06',  6, test37_06());
  perform assert('test37_07', 10, test37_07());
  perform assert('test38', 10, test38());
  perform assert('test38_01', 10, test38_01());
  perform assert('test38_02', 10, test38_02());
  perform assert('test38_03', 10, test38_03());
  perform assert('test39', 60, test39());
  perform assert('test40', 60, test40());
  perform assert('test41',100, test41());
  perform assert('test42', 91, test42());
  perform assert('test43',101, test43());
  perform assert('test44', 10, test44());
  perform assert('test50', 26, test50(3));
  perform assert('test51', 13, test51());
  perform assert('test52', 33, test52());
  perform assert('test53', 13, test53());
  perform assert('test53_01', 26, test53_01());
  perform assert('test54', -1, test54());
  perform assert('test55', -1, test55());
  perform assert('test56', -100, test56());
  perform assert('test56_01', -100, test56_01());
  perform assert('test57', -1, test57());
  perform assert('test58', 30, test58());
  perform assert('test58_01', 10, (test58_01()).a);
  perform assert('test59', 5348, test59(50));

  insert into footab values(1),(2),(3),(4);

  perform assert('test60', 10, test60());
  perform assert('test60_1', 10, test60_1());
  perform assert('test60_2', 10, test60_2());
  perform assert('test60_3', 100, test60_3());
  perform assert('test60_4', 100, test60_4());

  perform assert('test60', 10, test60());
  perform assert('test60_1', 10, test60_1());
  perform assert('test60_2', 10, test60_2());
  perform assert('test60_3', 100, test60_3());
  perform assert('test60_4', 100, test60_4());
  perform assert('test61', 10, test61(100));
  perform assert('test61', 30, test61(10));
  perform assert('test61_a', 10, test61(100));
  perform assert('test61_a', 30, test61(10));
  perform assert('test63', 3, test63(10));
  perform assert('test63',-1, test63(0));
  perform assert('test63_1', 3, test63_1(10));
  perform assert('test63_2', -1, test63_2(0));
  perform assert('test63_3', 1000002, test63_3(10));
  perform assert('test63_4', -2, test63_4(100000));
  perform assert('test64', 1, test64());
  perform assert('test64_1', 1, test64_1());
  perform assert('test64_2', 12, test64_2());
  perform assert('test64_3', 11, test64_3());
  perform assert('test64_33', 11, test64_33());
  perform assert('test64_4', 100, test64_4());
  perform assert('test64_5', 11, test64_5(0));
  perform assert('test64_5', 100, test64_5(1));
  perform assert('test64_5', 13, test64_5(2));
  perform assert('test65', 6, test65(10));
  perform assert('test65_1', 11, test65_1(-1));
  perform assert('test65_1', 105, test65_1(5));
  perform assert('test65_2', 11, test65_2(-1));
  perform assert('test65_2', 105, test65_2(5));
  perform assert('test65_3', 11, test65_3(-1));
  perform assert('test65_3', 205, test65_3(5));
  perform assert('test66', 0, test66(0));
  perform assert('test66', 2, test66(1));
  perform assert('test66', 3, test66(2));
  perform assert('test66', 5, test66(3));
  perform assert('test66', 6, test66(4));
  perform assert('test67', 7, test67(1));
  perform assert('test67', 6, test67(2));
  perform assert('test67', 101, test67(3));
  perform assert('test67_1', 7, test67_1(1));
  perform assert('test67_1', 6, test67_1(2));
  perform assert('test67_1', 101, test67_1(3));
  perform assert('test68', 102, test68());
  perform assert('test68_1', 103, test68_1());
  perform assert('test69', '02002', test69());
  perform assert('test69_1', '03002', test69_1());
  perform assert('test69_2', '00000', test69_2(10));
  perform assert('test69_2', '22012', test69_2(0));

  perform assert('test70', '02002', test70());
  perform assert('test70_1', 33554560, test70_1());
  perform assert('test70_2', 33554560, test70_2());
  perform assert('test71', 128, (test71())._sqlcode);
  perform assert('test71_1', 'HANDLED NOT FOUND, ALL IS OK', (test71_1())._message);
  perform assert('test71_2', 'My dynamic message 02000', (test71_2())._message);
  perform assert('test71_3', '00000', test71_3(10));
  perform assert('test71_3', '22012', test71_3(0));
  perform assert('test72','66550,33333', test72());
  perform assert('test72_1','66550,66550', test72_1());
  perform assert('test72_2','66550,44444', test72_2());
  perform assert('test72_3','45000,45000', test72_3());
  perform assert('test72_4','56444,56444', test72_4());
  perform assert('test73', ' 02005 00000 02005 02005 00000 02005', test73());
  perform assert('test73_1', ' 45005 00000 45005', test73_1());
  perform assert('test74', 'Signal handled', test74());
  perform assert('test74_1', 'not_found Signal handled', test74_1());
  perform assert('test74_2', 'not_found Signal handled 03000', test74_2());
  perform assert('test75', 1, test75(10));
  perform assert('test75', true, test75(0) is null);

  raise notice '******* All tests are ok *******';
end;
$$ language plpgsql;

select test();

DROP TYPE xx CASCADE;
drop schema regtest cascade;
set search_path = public;