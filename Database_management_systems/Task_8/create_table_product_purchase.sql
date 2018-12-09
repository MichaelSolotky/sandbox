CREATE TABLE `product_purchase` (
  `product_id` varchar(300) NOT NULL DEFAULT '',
  `purchase_id` varchar(20) NOT NULL DEFAULT '',
  `product_nm` varchar(300) DEFAULT NULL,
  `product_qty` decimal(10,3) DEFAULT NULL,
  `product_price` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`product_id`,`purchase_id`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;
