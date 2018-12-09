CREATE TABLE `customers` (
  `customer_id` int(11) NOT NULL,
  `first_nm` varchar(50) DEFAULT NULL,
  `second_nm` varchar(50) DEFAULT NULL,
  `gender` varchar(1) DEFAULT NULL,
  PRIMARY KEY (`customer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;
