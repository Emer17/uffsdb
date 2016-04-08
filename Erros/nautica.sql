create table classe(
cod integer not null primary key,
descr varchar(30) not null
);

create table velejador(
codv integer not null primary key,
nome varchar(30) not null,
dtnasc date not null,
classe integer not null,
constraint FK_velejador_classe foreign key (classe) references classe(cod)
);

create table barco(
codb integer not null primary key,
nome varchar(30) not null,
cor varchar(10) not null constraint CK_cor check (cor in ('blue','white','red','gray')),
tam integer not null,
pot integer not null
);

create table reserva(
codb integer not null,
codv integer not null,
data date not null,
dhoras float,
constraint PK_reserva primary key (codb, codv, data),
constraint FK_reserva_barco foreign key (codb) references barco(codb),
constraint FK_reserva_velejador foreign key (codv) references velejador(codv)
);

insert into classe(cod, descr)
values(10,'Amador'),
(20,'Iniciante'),
(30,'Pleno'),
(40,'Confirmado'),
(50,'Profissional');

insert into velejador(codv,nome,dtnasc,classe)
values(1,'joao','15/01/2000',10),
(2,'luiz','15/05/1990',20),
(3,'Caio','30/10/1989',30),
(4,'gabriel','25/09/1970',50),
(5,'felipe','14/06/1997',10),
(6,'lucas','23/12/1985',40),
(7,'vitor','07/01/1990',50),
(8,'cesar','13/12/1994',30),
(9,'jose','05/11/2002',20);

insert into barco(codb,nome,cor,tam,pot)
values(100,'fox','blue',20,30),
(200,'alfa','blue',8,20),
(300,'yankee','white',25,50),
(400,'juliet','white',9,18),
(500,'quebec','red',12,25),
(600,'zuluu','red',25,50),
(700,'voyager','gray',9,20),
(800,'wet house','gray',20,35);

insert into reserva(codb,codv,data,dhoras)
values(700,5,'03/07/2006',10),
(300,9,'10/10/2007',48),
(200,1,'10/02/2002',15),
(600,2,'18/09/2010',8),
(800,8,'02/05/2000',30),
(100,9,'14/10/2015',null),
(500,3,'25/04/1996',10),
(700,5,'02/02/2007',28);

--1. Retornar os nomes e a data de nascimento de todos os velejadores.
select v.nome, v.dtnasc from velejador v;

--2. Retornar os nomes e a descrição da classe de todos os velejadores.
select v.nome, c.descr from velejador v join classe c on v.classe = c.cod;

--3. Retornar os nomes e os tamanhos dos barcos de cor azul (Blue).
select b.nome, b.tam from barco b where b.cor = 'blue';

--4. Retornar dados dos velejadores e barcos que tiveram duração das reservas superiores a 20 horas.
select v.codv, v.nome, v.dtnasc, v.classe, b.codb, b.nome, b.cor, b.tam, b.pot from velejador v join
reserva r on r.dhoras > 20 and r.codv = v.codv join barco b on r.codb = b.codb;

select v.codv, v.nome, v.dtnasc, v.classe, b.codb, b.nome, b.cor, b.tam, b.pot from velejador v natural join
reserva r join barco b on r.codb = b.codb and r.dhoras > 20; --mesma coisa

--5. Retornar os dados dos barcos azuis cuja a potência seja superior a 30 HP.
select b.codb,b.nome, b.tam, b.pot from barco b where b.cor = 'blue' and b.pot > 30;

--6. Retornar os dados das reservas (apresentar os nomes ao invés dos códigos) feitas por velejadores iniciantes de barcos brancos (White).
select b.nome, v.nome, r.data, r.dhoras from reserva r natural join velejador v
join barco b on r.codb = b.codb where b.cor = 'white' and v.classe = 20;

--7. Retornar o nome dos velejadores com mais de quarenta anos (suponha que a função today() retorna a data atual).
SELECT * FROM (SELECT nome, date_part('year', age(velejador.dtnasc)) as idade FROM velejador) as result WHERE result.idade > 40;
-- ver outro jeito de fazer essa macumba (joao barp sabe fazer...)

--8. Retornar os dados dos barcos cujos os tamanhos sejam inferiores a 10 feet (pés).
select b.codb, b.nome, b.cor, b.tam, b.pot from barco b where b.tam < 10;

--9. Retornar a quantidade de reservas realizadas em 03/07/2006.
select r.codb, r.codv, r.dhoras from reserva r where r.data = '03/07/2006'; --nao esta certo
SELECT COUNT(*) as quantidade FROM reserva WHERE reserva.data = '03/07/2006'; --certo


--10. Retornar a média de potência dos barcos brancos.
SELECT AVG(pot) FROM barco WHERE cor='white';


--11. Retornar a maior potência dos barcos por cor.
SELECT cor, MAX(pot) FROM barco GROUP BY cor;

--12. Retornar a quantidade de velejadores por classe. Deve aparecer a descrição da classe e não o código.
SELECT classe.descr, result.quantidade FROM (select classe, count(*) as quantidade from velejador group by classe) 
as result JOIN classe ON result.classe = classe.cod;

--13. Retornar os dados de todos os barcos e de suas respectivas reservas, caso existirem.
select b.codb, b.nome, b.cor, b.tam, b.pot, v.nome, r.data, r.dhoras from barco b
natural join reserva r join velejador v on r.codv = v.codv; 

--14. Retornar os nomes dos velejadores e dos barcos que ainda não foram devolvidos.
select v.nome as velejador, b.nome as barco from reserva r natural join velejador v join barco b on r.codb = b.codb and r.dhoras is null;

--15. Retornar o nome do velejador e do barco que tiveram o maior tempo de reserva (podem existir mais de um).
SELECT v.nome, b.nome FROM velejador v NATURAL JOIN reserva r JOIN 
barco b ON b.codb=r.codb WHERE r.dhoras = (SELECT MAX(dhoras) FROM reserva);

--16. Retornar os dados dos barcos que nunca foram reservados.
SELECT * FROM barco WHERE NOT EXISTS (SELECT * FROM barco NATURAL JOIN reserva); --nao funciona

SELECT b.codb FROM barco b WHERE b.codb  NOT IN (SELECT reserva.codb FROM reserva); --funciona, como?

--17. Retornar o nome de todos os velejadores com as suas reservas caso existam.
select distinct v.nome from velejador v join reserva r on v.codv = r.codv; --errado
select v.nome,r.codb as barco,r.data,r.dhoras from velejador v left join reserva r on r.codv = v.codv; --certo

--18. Retornar o nome e classe (descrição) dos velejadores que fizeram menos de 10 reservas.

select * from
(select v.nome, count(r.codv) as qnt from velejador v left join reserva r on v.codv = r.codv group by v.nome) tb
where tb.qnt <10; --nao mostra a descriçao de classe

select v.nome, c.descr from velejador v left join reserva r on v.codv = r.codv join classe c on v.classe = c.cod group by v.nome, c.descr 
having count(*) < 10; --certo

--19. Retornar os nomes dos barcos com seus respectivos totais de tempo de reserva.
select b.nome, sum(r.dhoras) as total_horas from barco b join reserva r on r.codb = b.codb group by b.nome;

--20. Retornar os nomes dos velejadores e barcos reservados por velejadors amadores no ano de 2007.
select tb.nomeV, tb.nomeB from
(select v.nome as nomeV, v.classe, b.nome as nomeB from velejador v natural join reserva r join 
barco b on r.codb = b.codb where r.data > '01/01/2007' and r.data <= '31/12/2007')tb join
classe c on tb.classe = c.cod where c.descr = 'Amador';

--21. Retornar nome do velejador, descrição da classe do velejador, nome e cor do barco das reservas feitas pelo velejador.
select distinct v.nome as velejador,c.descr as classe,b.nome as barco,b.cor from velejador v join classe c on v.classe = c.cod natural join
reserva r join barco b on r.codb = b.codb;


--22. Retornar os dados do maior barco (ou maiores).
select * from barco where tam = (select max(tam) from barco);

--23. Retornar os dados dos velejadores que já reservaram o maior barco (ou maiores).
select v.* from velejador v natural join reserva r group by nome 
having count(*) = (select codb from barco where tam = (select max(tam) from barco)); --nao funciona

--24. Retornar o velejador mais novo da classe 'Profissional' (pode existir mais de um).

--25. Retornar o nome do velejador (ou velejadores) que reservaram todos os barcos vermelhos cadastrados.


--bonus: o numero de reservas feitas pelos velejadores.
select v.nome,count(*) from velejador v natural join reserva r group by v.nome; --order by 2 desc

--bonus: relaçao de velejadores com seu numero de reservas (mesmo que nao existam reservas)
select v.nome, count(r.codv) from velejador v left join reserva r on v.codv = r.codv group by v.nome;


--bonus: o nome do velejador com mais reservas (pode existir mais de um).
select v.nome from velejador v natural join reserva r group by v.nome 
having count(*) = (select max(qt) from(select codv, count(*) as qt from reserva group by 1) nres);


--bonus: barco de maior potencia
select nome from barco where pot = (select max(pot) from barco);



