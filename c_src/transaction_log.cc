// -------------------------------------------------------------------
// Copyright (c) 2016-2025 Benoit Chesneau. All Rights Reserved.
//
// This file is provided to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file
// except in compliance with the License.  You may obtain
// a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// -------------------------------------------------------------------

#include "erocksdb_db.h"
#include "transaction_log.h"

#include "refobjects.h"
#include "atoms.h"

namespace erocksdb {

TransactionLogHandler::TransactionLogHandler(ErlNifEnv* env)
    : rocksdb::WriteBatch::Handler()
{
    t_Env = env;
    t_List = enif_make_list(env, 0);
}

ERL_NIF_TERM
TransactionLogIterator(
    ErlNifEnv *env,
    int /*argc*/,
    const ERL_NIF_TERM argv[])
{

    ErlNifSInt64 since;
    std::unique_ptr<rocksdb::TransactionLogIterator> iter;
    ReferencePtr<DbObject> db_ptr;

    if(!enif_get_db(env, argv[0], &db_ptr))
        return enif_make_badarg(env);

    if(!enif_get_int64(env, argv[1], &since))
        return enif_make_badarg(env);

    rocksdb::SequenceNumber seq = since;
    rocksdb::Status status = db_ptr->m_Db->GetUpdatesSince(seq, &iter);

    if(!status.ok())
        return error_tuple(env, ATOM_ERROR, status);

    if(!iter->Valid())
    {
        return enif_make_tuple2(env, ATOM_ERROR, ATOM_INVALID_ITERATOR);;
    }

    auto itr_ptr = iter.release();

    TLogItrObject * tlog_ptr;
    tlog_ptr = TLogItrObject::CreateTLogItrObject(db_ptr.get(), itr_ptr);
    ERL_NIF_TERM result = enif_make_resource(env, tlog_ptr);
    enif_release_resource(tlog_ptr);
    iter = NULL;
    return enif_make_tuple2(env, ATOM_OK, result);
}

ERL_NIF_TERM
TransactionLogIteratorClose(
        ErlNifEnv* env,
        int /*argc*/,
        const ERL_NIF_TERM argv[])
{
    TLogItrObject * tlog_ptr;
    tlog_ptr=TLogItrObject::RetrieveTLogItrObject(env, argv[0]);
    if(NULL!=tlog_ptr)
    {
        ErlRefObject::InitiateCloseRequest(tlog_ptr);
        tlog_ptr = NULL;
        return ATOM_OK;
    }
    return enif_make_badarg(env);
}

ERL_NIF_TERM
TransactionLogNextUpdate(
    ErlNifEnv *env,
    int /*argc*/,
    const ERL_NIF_TERM argv[])
{
    const ERL_NIF_TERM& itr_handle_ref   = argv[0];

    ReferencePtr<TLogItrObject> itr_ptr;
    itr_ptr.assign(TLogItrObject::RetrieveTLogItrObject(env, itr_handle_ref));

    if(NULL==itr_ptr.get())
    {
        return enif_make_badarg(env);
    }

    rocksdb::TransactionLogIterator* itr = itr_ptr->m_Iter;

    if(!itr->Valid())
        return enif_make_tuple2(env, ATOM_ERROR, ATOM_INVALID_ITERATOR);

    rocksdb::Status status = itr->status();
    if (!status.ok()) {
        return error_tuple(env, ATOM_ERROR, status);
    }

    rocksdb::BatchResult batch = itr->GetBatch();
    TransactionLogHandler handler = TransactionLogHandler(env);
    batch.writeBatchPtr->Iterate(&handler);
    ERL_NIF_TERM log;
    enif_make_reverse_list(env, handler.t_List, &log);

    ERL_NIF_TERM binlog;
    std::string data = batch.writeBatchPtr->Data();
    unsigned char* value = enif_make_new_binary(env, data.size(), &binlog);
    memcpy(value, data.data(), data.size());

    itr->Next();

    return enif_make_tuple4(env,
                            ATOM_OK,
                            enif_make_int64(env, batch.sequence),
                            log,
                            binlog);
}

ERL_NIF_TERM
TransactionLogNextBinaryUpdate(
    ErlNifEnv *env,
    int /*argc*/,
    const ERL_NIF_TERM argv[])
{
    const ERL_NIF_TERM& itr_handle_ref = argv[0];

    ReferencePtr<TLogItrObject> itr_ptr;
    itr_ptr.assign(TLogItrObject::RetrieveTLogItrObject(env, itr_handle_ref));

    if(NULL==itr_ptr.get())
    {
        return enif_make_badarg(env);
    }

    rocksdb::TransactionLogIterator* itr = itr_ptr->m_Iter;

    if(!itr->Valid())
        return enif_make_tuple2(env, ATOM_ERROR, ATOM_INVALID_ITERATOR);

    rocksdb::Status status = itr->status();
    if (!status.ok()) {
        return error_tuple(env, ATOM_ERROR, status);
    }

    rocksdb::BatchResult batch = itr->GetBatch();
    ERL_NIF_TERM bindata;
    std::string data = batch.writeBatchPtr->Data();
    unsigned char* value = enif_make_new_binary(env, data.size(), &bindata);
    memcpy(value, data.data(), data.size());

    itr->Next();

    return enif_make_tuple3(env,
                            ATOM_OK,
                            enif_make_int64(env, batch.sequence),
                            bindata);
}

ERL_NIF_TERM
WriteBinaryUpdate(
    ErlNifEnv* env,
    int /*argc*/,
    const ERL_NIF_TERM argv[])
{
    ReferencePtr<DbObject> db_ptr;
    if(!enif_get_db(env, argv[0], &db_ptr))
        return enif_make_badarg(env);

    ErlNifBinary bin;
    if(!enif_inspect_binary(env, argv[1], &bin))
        return enif_make_badarg(env);

    rocksdb::WriteBatch batch(std::string((const char*)bin.data, bin.size));

    rocksdb::WriteOptions opts;
    fold(env, argv[2], parse_write_option, opts);

    rocksdb::Status status = db_ptr->m_Db->Write(opts, &batch);

    if (status.ok())
    {
        return ATOM_OK;
    }

    return error_tuple(env, ATOM_ERROR, status);
}

}
