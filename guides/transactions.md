For now only Erlang Rocksdb has preliminary support of Transactions using an OptimisticTransactionDB. 

## OptimisticTransactionDB

Optimistic Transactions provide light-weight optimistic concurrency control for workloads that do not expect high contention/interference between multiple transactions.

Optimistic Transactions do not take any locks when preparing writes. Instead, they rely on doing conflict-detection at commit time to validate that no other writers have modified the keys being written by the current transaction. If there is a conflict with another write (or it cannot be determined), the commit will return an error and no keys will be written.

Optimistic concurrency control is useful for many workloads that need to protect against occasional write conflicts. However, this many not be a good solution for workloads where write-conflicts occur frequently due to many transactions constantly attempting to update the same keys. For these workloads, using a TransactionDB may be a better fit. An OptimisticTransactionDB may be more performant than a TransactionDB for workloads that have many non-transactional writes and few transactions.

```erlang
{ok, Db, _} = rocksdb:open_optimistic_transaction_db(DbName, Options),
{ok, Transaction} = rocksdb:transaction(Db, []),
ok = rocksdb:transaction_put(Transaction, <<"key">>, <<"value">>),
ok = rocksdb:transaction_delete(Transaction, <<"key2">>),
ok = rocksdb:transaction_commit(Transaction),
```

## Reading from a Transaction


Transactions also support easily reading the state of keys that are currently batched in a given transaction but not yet committed:

```erlang
ok = rocksdb:put(Transaction, <<"a">>, <<"old">>),
ok = rocksdb:put(Transaction, <<"b">>, <<"old">>),

...
ok = rocksdb:transaction_put(Transaction, <<"a">>, <<"new">>

{ok, <<"new">>} = rocksdb:transaction_get(Transaction, <<"a">>, []),
{ok, <<"old">>} = rocksdb:transaction_get(Transaction, <<"b">>, []),
```





