create table classe(
cod integer primary key,
descr varchar(30)
);

create table velejador(
codv integer primary key,
nome varchar(30),
dtnasc varchar(8),                 /*nao há o tipo date*/
classe integer references classe(cod)
);

create table barco(
codb integer primary key,
nome varchar(30),
cor varchar(10),				  /*nao há constraint*/
tam integer ,
pot integer
);

create table reserva(			/*nao funciona. um att nao pode ser PK e FK ao mesmo tempo :S*/
codb integer primary key,		/*fazer estas linhas retorna: ERROR: column "codb" specified more than once*/		
codb integer references barco(codb),	
codv integer primary key,
codv integer references velejador(codv),
data varchar(8) primary key,
dhoras double
);
