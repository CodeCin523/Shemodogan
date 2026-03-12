#include <shemodogan/mem/shd_block.h>
#include <shemodogan/shd_core.h>


struct shd_dataact {
    shd_actor_meta_t *meta;
    shd_sbblock_t *instdt_alloc;
};
struct shd_datahnd {
    shd_handler_meta_t *meta;
    shd_basehnd_t      *instdt;
    shd_hndid_t         id;
};


/* ================================================================================ */
/*  CORE                                                                            */
/* ================================================================================ */

// Library Life-Cycle and Registers
shd_result_t shd_init_st(void) {

}
shd_result_t shd_term_st(void) {

}

shd_result_t shd_register_actor_st(shd_actid_t actid, shd_actor_meta_t *meta) {

}
shd_result_t shd_register_handler_st(shd_hndid_t hndid, shd_handler_meta_t *meta) {

}

// Handler Life-Cycle
static shd_result_t shd_handler_check_dependencies_st(
    shd_hndid_t hndid, shd_handler_meta_t *meta
) {
    
}
static shd_result_t shd_handler_check_dependents_st(
    shd_hndid_t hndid, shd_handler_meta_t *meta
) {

}

shd_result_t shd_handler_init_st(shd_hndid_t hndid, shd_basecrt_t *creator) {

}
shd_result_t shd_handler_term_st(shd_hndid_t hndid) {

}

shd_basehnd_t *shd_handler_get_mt(shd_hndid_t hndid, shd_basegtr_t *getter) {

}

// Actor Life-Cycle
shd_result_t shd_actor_subscribe_mt() {

}
shd_result_t shd_actor_release_mt() {

}

shd_baseact_t *shd_actor_get_mt() {

}