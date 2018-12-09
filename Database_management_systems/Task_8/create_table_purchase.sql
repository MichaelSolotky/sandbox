CREATE TABLE `purchase` (
  `purchare_id` varchar(20) NOT NULL DEFAULT '',
  `store_id` smallint(6) DEFAULT NULL,
  `customer_id` int(11) DEFAULT NULL,
  `loyalty_card_id` int(11) DEFAULT NULL,
  `bank_card_id` varchar(20) DEFAULT NULL,
  `voucher_id` varchar(20) DEFAULT NULL,
  `cashier_id` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`purchare_id`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;
