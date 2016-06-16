// aiScene* scenecopy_tmp;
// SceneCombiner::CopyScene(&scenecopy_tmp,pScene);

// std::unique_ptr<aiScene> scenecopy(scenecopy_tmp);
// const ScenePrivateData* const priv = ScenePriv(pScene);

// steps that are not idempotent, i.e. we might need to run them again, usually to get back to the
//                 // original state before the step was applied first. When checking which steps we don't need
//                                 // to run, those are excluded.
//                                                 const unsigned int nonIdempotentSteps = aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_MakeLeftHanded;
//
//                                                                 // Erase all pp steps that were already applied to this scene
//const unsigned int pp = (exp.mEnforcePP | pPreprocessing) & ~(priv && !priv->mIsCopy
//? (priv->mPPStepsApplied & ~nonIdempotentSteps)
// : 0u);

// If no extra postprocessing was specified, and we obtained this scene from an
// Assimp importer, apply the reverse steps automatically.
// TODO: either drop this, or document it. Otherwise it is just a bad surprise.
//if (!pPreprocessing && priv) {
//pp |= (nonIdempotentSteps & priv->mPPStepsApplied);


// If the input scene is not in verbose format, but there is at least postprocessing step that relies on it,
// we need to run the MakeVerboseFormat step first.
// bool must_join_again = false;
// if (!is_verbose_format) {

// bool verbosify = false;
// for( unsigned int a = 0; a < pimpl->mPostProcessingSteps.size(); a++) {
//BaseProcess* const p = pimpl->mPostProcessingSteps[a];

//if (p->IsActive(pp) && p->RequireVerboseFormat()) {
//verbosify = true;
//break;
//}
//}

// if (verbosify || (exp.mEnforcePP & aiProcess_JoinIdenticalVertices)) {
// DefaultLogger::get()->debug("export: Scene data not in verbose format, applying MakeVerboseFormat step first");

// MakeVerboseFormatProcess proc;
// proc.Execute(scene);

// if(!(exp.mEnforcePP & aiProcess_JoinIdenticalVertices)) {
// must_join_again = true;
// }
// }
// }

// if (pp) {
// the three 'conversion' steps need to be executed first because all other steps rely on the standard data layout
{
FlipWindingOrderProcess step;
//if (step.IsActive(pp)) {
step.Execute(scene);
// }
}

{
FlipUVsProcess step;
//if (step.IsActive(pp)) {
step.Execute(scene);
//}
}

{
MakeLeftHandedProcess step;
// if (step.IsActive(pp)) {
step.Execute(scene);
//}
}




/*
// dispatch other processes
for( unsigned int a = 0; a < pimpl->mPostProcessingSteps.size(); a++) {
BaseProcess* const p = pimpl->mPostProcessingSteps[a];

if (p->IsActive(pp)
&& !dynamic_cast<FlipUVsProcess*>(p)
&& !dynamic_cast<FlipWindingOrderProcess*>(p)
&& !dynamic_cast<MakeLeftHandedProcess*>(p)) {

p->Execute(scene);
}
}
ScenePrivateData* const privOut = ScenePriv(scene);
ai_assert(privOut);

privOut->mPPStepsApplied |= pp;
}

// if(must_join_again) {
// JoinVerticesProcess proc;
// proc.Execute(scene);
// }

ExportProperties emptyProperties;  // Never pass NULL ExportProperties so Exporters don't have to worry.
exp.mExportFunction(pPath,pimpl->mIOSystem.get(),scene, pProperties ? pProperties : &emptyProperties);
// }
// catch (DeadlyExportError& err) {
// pimpl->mError = err.what();
// return AI_FAILURE;
// }
