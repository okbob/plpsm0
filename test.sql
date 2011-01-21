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

create or replace function test06_06(a int)
returns int as $$
if a > 1 then
  return a * test06_06(a - 1);
else
  return 1;
end if;
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
  --set a.a = 40;
  if a.a = 40 then set s = s + 1; end if;
  set a = null;
  if a is null then set s = s + 1; end if;
  return s;
end;
$$ language psm0;

DROP TYPE xx;

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

create or replace function test30(out x int, out s int, out sqlcode int, out sqlstate char(5))
as $$
begin
  declare cx cursor for select a from footab;
  open cx;
  set s = 0;
  fetch cx into x;
  while sqlcode = 0 do
    set s = s + x;
    fetch cx into x;
    print s;
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

/*
 * NOT_FOUND event of FOR stmt isn't possible handled by custom
 * condition handlers!
 */
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


-- using a subselect in assign statement newer raise a handler
create or replace function test29()
returns int as $$ 
begin
  declare ret int default 100;
  declare continue handler for not found set ret = -100;
  set ret = (select a from footab limit 1); -- subselect just returns NULL;
  return coalesce(ret, -1);
end;
$$ language psm0; --returns -1

-- using a select into raise a handler
create or replace function test30()
returns int as $$
begin
  declare ret default 100;
  declare continue handler for not found set ret = -100;
  select a into ret from footab; -- raise error, when query returns more than one row
  return coalesce(ret, -1);
end;
$$ language psm0; -- returns -100;

create or replace function test31()
returns int as $$
begin
  declare ret default 100;
  select a into ret from footab;
  return coalesce(ret, -1);
end;
$$ language psm0; -- returns -1


/*************************************************
 * Assert functions
 */
create or replace function assert(text, int, int)
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
    raise exception 'test "%" broken', $1;
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
  perform assert('test27', 40, test27(5));
  perform assert('test28',  6, test28(4));
  perform assert('test30', 10, (select s from test30()));
  perform assert('test30', 128,(select sqlcode from test30()));
  perform assert('test30', '02000', (select sqlstate from test30()));
  perform assert('test31', 12, test31());
  perform assert('test32', 12, test32());
  perform assert('test32_01', 60, test32_01());
  perform assert('test33',  0, test33());
  perform assert('test34',  0, test34());
  perform assert('test35', 30, test35(10,20));
  perform assert('test35', 40, test35(0, (20,30)));
  perform assert('test36',  3, test36('footab'));
  perform assert('test36_01', 7, test36_01('footab'));
  perform assert('test37', 10, test37());
  perform assert('test37_01', 10, test37_01());
  perform assert('test37_02', 10, test37_02());
  perform assert('test37_03',  6, test37_03());
  perform assert('test37_04',  6, test37_04());
  perform assert('test37_05', 10, test37_05());
  perform assert('test37_06',  6, test37_06());
  perform assert('test37_07', 10, test37_07());
  perform assert('test38', 10, test38());
  perform assert('test39', 60, test39());
  perform assert('test40', 60, test40());
  perform assert('test41',100, test41());
  perform assert('test42', 91, test42());
  perform assert('test43',101, test43());
  perform assert('test44', 10, test44());
  perform assert('test50', 26, test50(3));

  raise notice '******* All tests are ok *******';
end;
$$ language plpgsql;


select test();

drop schema regtest cascade;
set search_path = public;