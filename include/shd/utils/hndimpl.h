#ifndef __SHD_UTILS_HANDLER_H__
#define __SHD_UTILS_HANDLER_H__

#endif

#define SHD_BUNDLE_ARGS(...) __VA_ARGS__

#define SHD_HNDIMPL_INITIALIZE_CREATOR(creator_ptr, creator_id, creator_type, creator_default) \
if(!creator_ptr) return SHD_STATUS_HANDLER_MISSING_CREATOR;                     \
if(creator_ptr->type != creator_id) return SHD_STATUS_HANDLER_INVALID_CREATOR;  \
creator_type creator = creator_default;                                         \
if((creator_ptr->flags & SHD_CRTFLAG_DEFAULT_CREATOR) == 0) {                   \
    creator_type *creator_data = (creator_type *) creator_ptr;                  \
    creator = *creator_data;                                                    \
}
#define SHD_HNDIMPL_INITIALIZE_DTINST(handler_ptr, handler_id, handler_type) \
handler_ptr = (handler_type *) shd_handler_get(                                 \
    handler_id,                                                                 \
    &(shd_basegtr_t){ 0, SHD_GTRFLAG_DIRECT_INSTANCE }                          \
);                                                                              \
if(!handler_ptr) return SHD_STATUS_FAILED;

#define SHD_HNDIMPL_TEMPSTACK_ACCESS(stack_ptr) \
extern shd_dfstack_t g_hndtemp_stack; stack_ptr = &g_hndtemp_stack
#define SHD_HNDIMPL_STATICSTACK_ACCESS(stack_ptr) \
extern shd_dfstack_t g_hndstatic_stack; stack_ptr = &g_hndstatic_stack


#ifdef SHD_UTILS_UNDEF
#undef SHD_HNDIMPL_INITIALIZE_CREATOR
#undef SHD_HNDIMPL_INITIALIZE_DTINST
#undef SHD_HNDIMPL_TEMPSTACK_ACCESS
#endif /* __SHD_UTILS_HANDLER_H__ */