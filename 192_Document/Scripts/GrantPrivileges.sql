#��ʼ�����ݿ��ʺź�Ȩ��

#insert into mysql.user(Host,User,Password) values("*","bymobile",password("bymobile20170605"));

GRANT ALL PRIVILEGES ON *.* TO 'bymobile'@'%' IDENTIFIED BY 'bymobile20170605' WITH GRANT OPTION; 
GRANT ALL PRIVILEGES ON *.* TO 'bymobile'@'localhost' IDENTIFIED BY 'bymobile20170605' WITH GRANT OPTION; 

flush privileges;
