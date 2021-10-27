# 
# Structure for table `users`
#

# role-----0.user 1.manager 2.root
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users`
(
    `Id`        varchar(24) NOT NULL,
    `username`  varchar(64) NOT NULL,
    `passwd`    varchar(64) NOT NULL,
    `role`      INT,
    `crt_time`  datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
    PRIMARY KEY (`Id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

# sha224('abc')='23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7'
INSERT INTO `users` VALUES('9217916635379004','user_01','23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7',0,'2021-09-17 13:46:51');
INSERT INTO `users` VALUES('0610466290923301','manager_01','23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7',1,'2021-09-17 13:46:51');
INSERT INTO `users` VALUES('9250707949178507','root','23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7',2,'2021-09-17 13:46:51');


#
# Structure for table `cards`
#
# status : Normal  Frozen  Lost 
DROP TABLE IF EXISTS `cards`;
CREATE TABLE `cards`
(
    `card_num`  varchar(24) NOT NULL,
    `user`      varchar(64) NOT NULL,
    `balance`   BIGINT,
    `status`    varchar(32),
    PRIMARY KEY (`card_num`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;





#
# Structure fot table `cardopt`
#
#  card_num  Operator  operation date
DROP TABLE IF EXISTS `cardopt`;
CREATE TABLE `cardopt`
(
    `id`        BIGINT   AUTO_INCREMENT,
    `card_num`  varchar(24) NOT NULL,
    `operator`  varchar(64) NOT NULL,
    `operation` varchar(64) NOT NULL,
    `opt_time`  datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
    PRIMARY KEY (`id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;