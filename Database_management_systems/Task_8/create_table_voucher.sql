CREATE TABLE `voucher` (
  `voucher_id` varchar(20) NOT NULL DEFAULT '',
  `purchase_time` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`voucher_id`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251;
