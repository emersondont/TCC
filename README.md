# TCC - Investigação de Sensores Virtuais Tolerantes à Falha para o Monitoramento do Uso de Salas em um Campus Inteligente

Este repositório contém o código fonte e a documentação relacionados ao Trabalho de Conclusão de Curso (TCC) intitulado "Investigação de Sensores Virtuais Tolerantes à Falha para o Monitoramento do Uso de Salas no Contexto de um Campus Inteligente". Este projeto é parte da pesquisa que visa desenvolver um sistema IoT capaz de monitorar a ocupação de salas de aula em tempo real, utilizando sensores virtuais e o protocolo de comunicação MQTT.

## Resumo
A evolução da tecnologia, impulsionada pela capacidade de processamento e comunicação entre dispositivos computacionais, deu origem à Internet das Coisas (IoT), possibilitando a automação de serviços e o surgimento da Indústria 4.0. No contexto da IoT, surge o conceito de sensor virtual, que pode simular a funcionalidade de um ou mais sensores físicos, monitorando uma mesma variável do ambiente ou diferentes variáveis correlacionadas. Isso torna possível inferir informações em caso de falha de um sensor físico, tornando o sistema tolerante a falhas.

Atualmente, na UFPel, mais especificamente no Campus Porto (Anglo), identificou-se a necessidade de melhorar o serviço de supervisão e gerenciamento das salas de aula e demais espaços acadêmicos ao longo do semestre. O controle e gestão atual das salas não permitem a identificação de salas não utilizadas ou pouco utilizadas durante o semestre letivo, resultando em má utilização dos recursos disponíveis. Esses problemas afetam a qualidade do ensino, causando alocações concentradas em horários inadequados, conflitos de agendamento e dificuldades de acesso a recursos para atividades eventuais, mesmo quando os recursos estão subutilizados.

Portanto, este trabalho tem como objetivo desenvolver um sistema IoT tolerante a falhas, explorando o conceito de sensor virtual para monitorar a ocupação de salas de aula e laboratórios no Campus Anglo. O uso de sensores virtuais permite a identificação em tempo real da ocupação ou não dos espaços acadêmicos a partir de um conjunto de sensores físicos que monitoram diferentes variáveis do ambiente. Isso é possível mesmo com a falha de algum sensor físico, graças a um algoritmo proposto.

O projeto inicialmente investiga variáveis do ambiente que podem indicar a ocupação da sala para atividades acadêmicas, tais como sensores de luminosidade, ruído sonoro, movimento de pessoas e ultrassônico para avaliação de distância. Em seguida, é proposto um algoritmo capaz de decidir sobre a ocupação ou não do ambiente com base nos resultados individuais dos sensores.

Os resultados obtidos pelo monitoramento de um laboratório de pesquisa demonstram a eficiência da solução proposta, mesmo na ausência de uma variável do ambiente.

## Conteúdo do Repositório

Este repositório contém os seguintes componentes principais:

1. **Código-fonte do Publisher (C++)**: O código para o dispositivo ESP32 que publica os dados dos sensores no tópico MQTT. Também contém o desenvolvimento do Sensor Virtual.

2. **Código-fonte do Subscriber (Python)**: O código que assina o tópico MQTT e armezena os dados recebidos, permitindo o monitoramento da ocupação das salas.
