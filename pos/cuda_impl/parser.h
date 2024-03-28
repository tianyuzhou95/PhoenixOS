#pragma once

#include <cuda_runtime_api.h>

#include "pos/include/common.h"
#include "pos/include/workspace.h"
#include "pos/include/client.h"
#include "pos/include/transport.h"
#include "pos/include/parser.h"
#include "pos/cuda_impl/client.h"

#include "pos/cuda_impl/api_index.h"

namespace ps_functions {
    /* CUDA runtime functions */
    POS_PS_DECLARE_FUNCTIONS(cuda_malloc);
    POS_PS_DECLARE_FUNCTIONS(cuda_free);
    POS_PS_DECLARE_FUNCTIONS(cuda_launch_kernel);
    POS_PS_DECLARE_FUNCTIONS(cuda_memcpy_h2d);
    POS_PS_DECLARE_FUNCTIONS(cuda_memcpy_d2h);
    POS_PS_DECLARE_FUNCTIONS(cuda_memcpy_d2d);
    POS_PS_DECLARE_FUNCTIONS(cuda_memcpy_h2d_async);
    POS_PS_DECLARE_FUNCTIONS(cuda_memcpy_d2h_async);
    POS_PS_DECLARE_FUNCTIONS(cuda_memcpy_d2d_async);
    POS_PS_DECLARE_FUNCTIONS(cuda_set_device);
    POS_PS_DECLARE_FUNCTIONS(cuda_get_last_error);
    POS_PS_DECLARE_FUNCTIONS(cuda_get_error_string);
    POS_PS_DECLARE_FUNCTIONS(cuda_peek_at_last_error);
    POS_PS_DECLARE_FUNCTIONS(cuda_get_device_count);
    POS_PS_DECLARE_FUNCTIONS(cuda_get_device_properties);
    POS_PS_DECLARE_FUNCTIONS(cuda_device_get_attribute);
    POS_PS_DECLARE_FUNCTIONS(cuda_get_device);
    POS_PS_DECLARE_FUNCTIONS(cuda_func_get_attributes);
    POS_PS_DECLARE_FUNCTIONS(cuda_occupancy_max_active_bpm_with_flags);
    POS_PS_DECLARE_FUNCTIONS(cuda_stream_synchronize);
    POS_PS_DECLARE_FUNCTIONS(cuda_stream_is_capturing);
    POS_PS_DECLARE_FUNCTIONS(cuda_event_create_with_flags);
    POS_PS_DECLARE_FUNCTIONS(cuda_event_destory);
    POS_PS_DECLARE_FUNCTIONS(cuda_event_record);
    
    /* CUDA driver functions */
    POS_PS_DECLARE_FUNCTIONS(__register_function);   
    POS_PS_DECLARE_FUNCTIONS(cu_module_load); 
    POS_PS_DECLARE_FUNCTIONS(cu_module_load_data);    
    POS_PS_DECLARE_FUNCTIONS(cu_module_get_function);
    POS_PS_DECLARE_FUNCTIONS(cu_module_get_global);
    POS_PS_DECLARE_FUNCTIONS(cu_ctx_get_current);
    POS_PS_DECLARE_FUNCTIONS(cu_device_primary_ctx_get_state);    

    /* cuBLAS functions */
    POS_PS_DECLARE_FUNCTIONS(cublas_create);
    POS_PS_DECLARE_FUNCTIONS(cublas_set_stream);
    POS_PS_DECLARE_FUNCTIONS(cublas_set_math_mode);
    POS_PS_DECLARE_FUNCTIONS(cublas_sgemm);
    POS_PS_DECLARE_FUNCTIONS(cublas_sgemm_strided_batched);

    /* remoting functions */
    POS_PS_DECLARE_FUNCTIONS(remoting_deinit);
} // namespace ps_functions

/*!
 *  \brief  POS Runtime (CUDA Implementation)
 *  \note   1. Parser:      parsing each API call, translate virtual handles to physicall handles;
 *          2. DAG:         maintainance of launch flow for checkpoint/restore and scheduling;
 *          3. Scheduler:   launch unfinished / previously-failed call to worker
 */
class POSParser_CUDA : public POSParser {
 public:
    POSParser_CUDA(POSWorkspace* ws) : POSParser(ws){}
    ~POSParser_CUDA() = default;

 private:
    /*!
     *  \brief      initialization of the runtime daemon thread
     *  \example    for CUDA, one need to call API e.g. cudaSetDevice first to setup the context for a thread
     */
    pos_retval_t daemon_init() override {
        // /*!
        //  *  \note   make sure the worker thread is bound to a CUDA context
        //  *          if we don't do this and use the driver API, it might be unintialized
        //  */
        // if(cudaSetDevice(0) != cudaSuccess){
        //     POS_WARN_C_DETAIL("runtime thread failed to invoke cudaSetDevice");
        //     return POS_FAILED; 
        // }
        // cudaDeviceSynchronize();
        return POS_SUCCESS; 
    }

    /*!
     *  \brief  insertion of parse and dag functions
     *  \return POS_SUCCESS for succefully insertion
     */
    pos_retval_t init_ps_functions() override {
        this->_parser_functions.insert({
            /* CUDA runtime functions */
            {   CUDA_MALLOC,                    ps_functions::cuda_malloc::parse                        },
            {   CUDA_FREE,                      ps_functions::cuda_free::parse                          },
            {   CUDA_LAUNCH_KERNEL,             ps_functions::cuda_launch_kernel::parse                 },
            {   CUDA_MEMCPY_HTOD,               ps_functions::cuda_memcpy_h2d::parse                    },
            {   CUDA_MEMCPY_DTOH,               ps_functions::cuda_memcpy_d2h::parse                    },
            {   CUDA_MEMCPY_DTOD,               ps_functions::cuda_memcpy_d2d::parse                    },
            {   CUDA_MEMCPY_HTOD_ASYNC,         ps_functions::cuda_memcpy_h2d_async::parse              },
            {   CUDA_MEMCPY_DTOH_ASYNC,         ps_functions::cuda_memcpy_d2h_async::parse              },
            {   CUDA_MEMCPY_DTOD_ASYNC,         ps_functions::cuda_memcpy_d2d_async::parse              },
            {   CUDA_SET_DEVICE,                ps_functions::cuda_set_device::parse                    },
            {   CUDA_GET_LAST_ERROR,            ps_functions::cuda_get_last_error::parse                },
            {   CUDA_GET_ERROR_STRING,          ps_functions::cuda_get_error_string::parse              },
            {   CUDA_PEEK_AT_LAST_ERROR,        ps_functions::cuda_peek_at_last_error::parse            },
            {   CUDA_GET_DEVICE_COUNT,          ps_functions::cuda_get_device_count::parse              },
            {   CUDA_GET_DEVICE_PROPERTIES,     ps_functions::cuda_get_device_properties::parse         },
            {   CUDA_DEVICE_GET_ATTRIBUTE,      ps_functions::cuda_device_get_attribute::parse          },
            {   CUDA_GET_DEVICE,                ps_functions::cuda_get_device::parse                    },
            {   CUDA_FUNC_GET_ATTRIBUTES,       ps_functions::cuda_func_get_attributes::parse           },
            {   CUDA_OCCUPANCY_MAX_ACTIVE_BPM_WITH_FLAGS,   
                                        ps_functions::cuda_occupancy_max_active_bpm_with_flags::parse   },
            {   CUDA_STREAM_SYNCHRONIZE,        ps_functions::cuda_stream_synchronize::parse            },
            {   CUDA_STREAM_IS_CAPTURING,       ps_functions::cuda_stream_is_capturing::parse           },
            {   CUDA_EVENT_CREATE_WITH_FLAGS,   ps_functions::cuda_event_create_with_flags::parse       },
            {   CUDA_EVENT_DESTROY,             ps_functions::cuda_event_destory::parse                 },
            {   CUDA_EVENT_RECORD,              ps_functions::cuda_event_record::parse                  },
            /* CUDA driver functions */
            {   rpc_cuModuleLoad,               ps_functions::cu_module_load::parse                     },
            {   rpc_cuModuleLoadData,           ps_functions::cu_module_load_data::parse                },
            {   rpc_register_function,          ps_functions::__register_function::parse                },
            {   rpc_cuModuleGetFunction,        ps_functions::cu_module_get_function::parse             },
            {   rpc_register_var,               ps_functions::cu_module_get_global::parse               },
            {   rpc_cuCtxGetCurrent,            ps_functions::cu_ctx_get_current::parse                 },
            {   rpc_cuDevicePrimaryCtxGetState, ps_functions::cu_device_primary_ctx_get_state::parse    },
            {   rpc_cuLaunchKernel,             ps_functions::cuda_launch_kernel::parse                 },
            
            /* cuBLAS functions */
            {   rpc_cublasCreate,               ps_functions::cublas_create::parse                      },
            {   rpc_cublasSetStream,            ps_functions::cublas_set_stream::parse                  },
            {   rpc_cublasSetMathMode,          ps_functions::cublas_set_math_mode::parse               },
            {   rpc_cublasSgemm,                ps_functions::cublas_sgemm::parse                       },
            {   rpc_cublasSgemmStridedBatched,  ps_functions::cublas_sgemm_strided_batched::parse       },

            /* remoting functgions */
            {   rpc_deinit,                     ps_functions::remoting_deinit::parse                    },
        });
        POS_DEBUG_C("insert %lu runtime parse functions", this->_parser_functions.size());

        return POS_SUCCESS;
    }

    /*!
     *  \brief  naive implementation of checkpoint insertion procedure
     *  \note   this implementation naively insert a checkpoint op to the dag, 
     *          without any optimization hint
     *  \param  wqe the exact WQ element before inserting checkpoint op
     *  \return POS_SUCCESS for successfully checkpoint insertion
     */
    pos_retval_t __checkpoint_insertion_naive(POSAPIContext_QE* wqe) { 
        pos_retval_t retval = POS_SUCCESS;
        POSHandle *handle;
        POSAPIContext_QE *ckpt_wqe;

        ckpt_wqe = new POSAPIContext_QE_t(
            /* api_id*/ this->_ws->checkpoint_api_id,
            /* client */ wqe->client
        );
        POS_CHECK_POINTER(ckpt_wqe);
        retval = ((POSClient*)wqe->client)->dag.launch_op(ckpt_wqe);

    exit:
        return retval;
    }

    /*!
     *  \brief  level-1/2 optimization of checkpoint insertion procedure
     *  \note   this implementation give hints of those memory handles that
     *          been modified (INOUT/OUT) since last checkpoint
     *  \param  wqe the exact WQ element before inserting checkpoint op
     *  \return POS_SUCCESS for successfully checkpoint insertion
     */
    pos_retval_t __checkpoint_insertion_incremental(POSAPIContext_QE* wqe) {
        pos_retval_t retval = POS_SUCCESS;
        POSClient_CUDA *client;
        POSHandleManager<POSHandle>* hm;
        POSAPIContext_QE *ckpt_wqe;
        uint64_t i;

        POS_CHECK_POINTER(wqe);

        client = (POSClient_CUDA*)(wqe->client);
        
        ckpt_wqe = new POSAPIContext_QE_t(
            /* api_id*/ this->_ws->checkpoint_api_id,
            /* client */ wqe->client
        );
        POS_CHECK_POINTER(ckpt_wqe);

        /*!
         *  \note   we only checkpoint those resources that has been modified since last checkpoint
         */
        for(auto &stateful_handle_id : this->_ws->stateful_handle_type_idx){
            hm = pos_get_client_typed_hm(client, stateful_handle_id, POSHandleManager<POSHandle>);
            POS_CHECK_POINTER(hm);
            std::set<POSHandle*>& modified_handles = hm->get_modified_handles();
            if(likely(modified_handles.size() > 0)){
                ckpt_wqe->record_checkpoint_handles(modified_handles);
            }
            hm->clear_modified_handle();
        }

        retval = ((POSClient*)wqe->client)->dag.launch_op(ckpt_wqe);
        
    exit:
        return retval;
    }

    /*!
     *  \brief  insert checkpoint op to the DAG based on certain conditions
     *  \note   aware of the macro POS_CKPT_ENABLE_INCREMENTAL
     *  \param  wqe the exact WQ element before inserting checkpoint op
     *  \return POS_SUCCESS for successfully checkpoint insertion
     */
    pos_retval_t checkpoint_insertion(POSAPIContext_QE* wqe) override {
        #if POS_CKPT_OPT_LEVEL > 0
            #if POS_CKPT_ENABLE_INCREMENTAL == 1
                return __checkpoint_insertion_incremental(wqe);
            #else
                return __checkpoint_insertion_naive(wqe);
            #endif
        #else
            // insert no checkpoint op, so we won't trigger any checkpoint
            return POS_SUCCESS;
        #endif
    }
};
