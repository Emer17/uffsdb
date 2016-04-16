create database relatorio_vendas;

\c relatorio_vendas;

create table lojas(
cod integer primary key,
cidade varchar(30),
regiao varchar(30)
);

create table produtos(
cod integer primary key,
descricao varchar(30),
estudio varchar(30) 
);

create table vendas(
id integer primary key,
loja integer references lojas(cod),
produto integer references produtos(cod),
venda double,
custo double,
lucro double
);

insert into lojas (cod,cidade,regiao) values (1,'New York','East');
insert into lojas (cod,cidade,regiao) values (2,'Chicago','Central');
insert into lojas (cod,cidade,regiao) values (3,'Atlanta','East');
insert into lojas (cod,cidade,regiao) values (4,'Los Angeles','West');
insert into lojas (cod,cidade,regiao) values (5,'San Francisco','WEst');
insert into lojas (cod,cidade,regiao) values (6,'Philadelphia','East');

insert into produtos (cod,descricao,estudio) values (1,'Beautiful Girls','MKF Studios');
insert into produtos (cod,descricao,estudio) values (2,'Toy Story','Wolf');
insert into produtos (cod,descricao,estudio) values (3,'Sense and Sensibility','Parabuster Inc.');
insert into produtos (cod,descricao,estudio) values (4,'Holiday of the Year','Wolf');
insert into produtos (cod,descricao,estudio) values (5,'Puul Fiction','MKF Studios');
insert into produtos (cod,descricao,estudio) values (6,'The Juror','MKF Studios');
insert into produtos (cod,descricao,estudio) values (7,'From Dusk till Down','Parabuster Inc.');
insert into produtos (cod,descricao,estudio) values (8,'Hellraiser: Bloodline','Big Studios');

insert into vendas (id,loja,produto,venda,custo,lucro) values (1, 1, 6, 2.39, 1.15, 1.24);
insert into vendas (id,loja,produto,venda,custo,lucro) values (2, 1, 2, 16.7, 6.91, 9.79);
insert into vendas (id,loja,produto,venda,custo,lucro) values (3, 2, 7, 7.16, 2.75, 4.40);
insert into vendas (id,loja,produto,venda,custo,lucro) values (4, 3, 2, 4.47, 1.84, 2.93);
insert into vendas (id,loja,produto,venda,custo,lucro) values (5, 5, 3, 11.93, 4.59, 7.34);
insert into vendas (id,loja,produto,venda,custo,lucro) values (6, 5, 1, 14.31, 5.51, 8.80);
