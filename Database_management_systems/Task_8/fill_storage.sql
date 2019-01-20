insert into s_gr1.cashier (cashier_id) select distinct cashier_id from srcdt.purchase

insert into s_gr1.customers (customer_id, first_nm, second_nm, gender)
select distinct loyal_card_id, first_nm, last_nm, gender_cd
from srcdt.loyal_card

insert into s_gr1.loyalty_card (loyalty_card_id)
select distinct loyal_card_id
from srcdt.loyal_card

insert into s_gr1.product_purchase (product_id, purchase_id, product_nm, product_qty, product_price)
select  product_nm as idd, voucher_id as idd2, product_nm, (product_qty), (sum(position_amt) + sum(discount_amt)) / sum(product_qty)
from srcdt.cash_voucher
group by idd2, idd



insert into s_gr1.store (store_id, store_nm, store_num)
select distinct concat_ws('#', store_id, store_nm), store_nm, store_id from srcdt.purchase

insert into s_gr1.bank_card (bank_card_id)
select distinct bank_card_id from srcdt.purchase

insert into s_gr1.voucher (voucher_id, voucher_dttm) #, voucher_dttm)
select distinct voucher_id, voucher_dttm from srcdt.purchase

select distinct voucher_id, concat_ws('#', store_id, store_nm), loyal_card_id, loyal_card_id, bank_card_id, voucher_id, cashier_id
from srcdt.purchase