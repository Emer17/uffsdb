create database relatorio_vendas;

\c relatorio_vendas;

create table lojas(
cod integer not null primary key,
cidade varchar(30) not null,
regiao varchar(30) not null
);

create table produtos(
cod integer not null primary key,
descricao varchar(30) not null,
estudio varchar(30) not null
);

create table vendas(
id integer not null primary key,
loja integer not null,
produto integer not null,
venda float not null,
custo float not null,
lucro float not null,
constraint FK_vendas_lojas foreign key (loja) references lojas(cod),
constraint FK_vendas_produtos foreign key (produto) references produtos(cod)
);

insert into lojas (cod,cidade,regiao) values
(1,'New York','East'),
(2,'Chicago','Central'),
(3,'Atlanta','East'),
(4,'Los Angeles','West'),
(5,'San Francisco','WEst'),
(6,'Philadelphia','East');

insert into produtos (cod,descricao,estudio) values
(1,'Beautiful Girls','MKF Studios'),
(2,'Toy Story','Wolf'),
(3,'Sense and Sensibility','Parabuster Inc.'),
(4,'Holiday of the Year','Wolf'),
(5,'Puul Fiction','MKF Studios'),
(6,'The Juror','MKF Studios'),
(7,'From Dusk till Down','Parabuster Inc.'),
(8,'Hellraiser: Bloodline','Big Studios');

insert into vendas (id,loja,produto,venda,custo,lucro) values
(1, 1, 6, 2.39, 1.15, 1.24),
(2, 1, 2, 16.7, 6.91, 9.79),
(3, 2, 7, 7.16, 2.75, 4.40),
(4, 3, 2, 4.47, 1.84, 2.93),
(5, 5, 3, 11.93, 4.59, 7.34),
(6, 5, 1, 14.31, 5.51, 8.80);

select p.descricao, p.estudio from produtos p; /*nao funciona*/
