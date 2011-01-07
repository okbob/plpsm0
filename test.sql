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

/*************************************************
 *
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

create or replace function test()
returns void as $$
begin
  perform assert('test01', -1, test01(11));
  perform assert('test02', -1, test02(11));
  perform assert('test03', -5, test03(11));
  perform assert('test04', 45, test04(10));
  perform assert('test05', 11, test05(11));
  perform assert('test06', 'Hello Hello', test06('Hello'));
  perform assert('test07',  5, test07(3));
  perform assert('test08',  3, test08(3));
  perform assert('test09', 15, test09(3));
  perform assert('test10', 15, test10(3));
  perform assert('test11', 13, test11(10));
  perform assert('test12',  4, test12(1));
  perform assert('test13', 10, test13(0));
  perform assert('test14', 2, (test14()).b);

  raise notice '******* All tests are ok *******';
end;
$$ language plpgsql;


select test();

drop schema regtest cascade;
set search_path = public;